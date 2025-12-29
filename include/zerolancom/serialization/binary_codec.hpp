#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace zlc
{

// =======================
// Zero-copy view for decode
// =======================

struct ByteView
{
  const uint8_t *data{nullptr};
  size_t size{0};

  const uint8_t *begin() const;
  const uint8_t *end() const;
};

// =======================
// Owning buffer (C-style)
// =======================

struct ByteBuffer
{
  uint8_t *data{nullptr};
  size_t size{0};
  size_t capacity{0};

  ~ByteBuffer();

  void write(const char *buf, size_t len);
};

// =======================
// Writer
// =======================

struct BinWriter
{
  std::vector<uint8_t> buf;

  void write_u16(uint16_t v);
  void write_u32(uint32_t v);
  void write_string(const std::string &s);
  void write_fixed_string(const std::string &s, size_t len);
};

// =======================
// Reader (zero-copy)
// =======================

struct BinReader
{
  ByteView view;

  uint16_t read_u16();
  uint32_t read_u32();
  std::string read_string();
  std::string read_fixed_string(size_t len);
};

// =======================
// Utilities
// =======================

std::string decodeServiceHeader(ByteView payload);

} // namespace zlc
