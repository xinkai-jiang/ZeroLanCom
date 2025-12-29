#include "serialization/binary_codec.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace zlc
{

/* ================= ByteView ================= */

const uint8_t *ByteView::begin() const
{
  return data;
}

const uint8_t *ByteView::end() const
{
  return data + size;
}

/* ================= ByteBuffer ================= */

ByteBuffer::~ByteBuffer()
{
  std::free(data);
}

void ByteBuffer::write(const char *buf, size_t len)
{
  if (size + len > capacity)
  {
    size_t newcap = std::max(capacity * 2, size + len);
    uint8_t *newdata = static_cast<uint8_t *>(std::realloc(data, newcap));
    if (!newdata)
      throw std::bad_alloc();
    data = newdata;
    capacity = newcap;
  }
  std::memcpy(data + size, buf, len);
  size += len;
}

/* ================= BinWriter ================= */

void BinWriter::write_u16(uint16_t v)
{
  buf.push_back(static_cast<uint8_t>(v >> 8));
  buf.push_back(static_cast<uint8_t>(v & 0xFF));
}

void BinWriter::write_u32(uint32_t v)
{
  buf.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
  buf.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
  buf.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
  buf.push_back(static_cast<uint8_t>(v & 0xFF));
}

void BinWriter::write_string(const std::string &s)
{
  write_u16(static_cast<uint16_t>(s.size()));
  buf.insert(buf.end(), s.begin(), s.end());
}

void BinWriter::write_fixed_string(const std::string &s, size_t len)
{
  if (s.size() != len)
    throw std::runtime_error("fixed string length mismatch");
  buf.insert(buf.end(), s.begin(), s.end());
}

/* ================= BinReader ================= */

uint16_t BinReader::read_u16()
{
  if (view.size < 2)
    throw std::runtime_error("decode u16 OOB");
  uint16_t v = (view.data[0] << 8) | view.data[1];
  view.data += 2;
  view.size -= 2;
  return v;
}

uint32_t BinReader::read_u32()
{
  if (view.size < 4)
    throw std::runtime_error("decode u32 OOB");
  uint32_t v = (uint32_t(view.data[0]) << 24) | (uint32_t(view.data[1]) << 16) |
               (uint32_t(view.data[2]) << 8) | uint32_t(view.data[3]);
  view.data += 4;
  view.size -= 4;
  return v;
}

std::string BinReader::read_string()
{
  uint16_t len = read_u16();
  if (view.size < len)
    throw std::runtime_error("decode string OOB");
  std::string s(reinterpret_cast<const char *>(view.data), len);
  view.data += len;
  view.size -= len;
  return s;
}

std::string BinReader::read_fixed_string(size_t len)
{
  if (view.size < len)
    throw std::runtime_error("decode fixed string OOB");
  std::string s(reinterpret_cast<const char *>(view.data), len);
  view.data += len;
  view.size -= len;
  return s;
}

/* ================= Utilities ================= */

std::string decodeServiceHeader(ByteView payload)
{
  constexpr size_t kMaxLen = 1024;

  if (!payload.data || payload.size == 0)
    return {};

  size_t real_len = 0;
  while (real_len < payload.size && real_len < kMaxLen && payload.data[real_len] != 0)
  {
    ++real_len;
  }

  return std::string(reinterpret_cast<const char *>(payload.data), real_len);
}

} // namespace zlc
