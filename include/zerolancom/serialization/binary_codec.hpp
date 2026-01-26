#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace zlc
{

using Bytes = std::vector<uint8_t>;

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
// Utilities
// =======================

std::string decodeServiceHeader(ByteView payload);

} // namespace zlc
