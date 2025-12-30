#pragma once
#include "zerolancom/serialization/binary_codec.hpp"
#include "zerolancom/utils/message.hpp"
#include <msgpack.hpp>

// TODO: pretect for encode and decode failure cases using try-catch
namespace zlc
{

template <typename T> inline void encode(const T &obj, ByteBuffer &out)
{
  out.size = 0;
  msgpack::packer<ByteBuffer> pk(out);
  pk.pack(obj);
}

template <typename T> inline void decode(const ByteView &bv, T &out)
{
  msgpack::object_handle oh =
      msgpack::unpack(reinterpret_cast<const char *>(bv.data), bv.size);
  oh.get().convert(out);
}

} // namespace zlc
