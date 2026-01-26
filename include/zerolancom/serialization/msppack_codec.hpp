#pragma once

#include <msgpack.hpp>

#include "zerolancom/serialization/binary_codec.hpp"
#include "zerolancom/utils/exception.hpp"
#include "zerolancom/utils/message.hpp"

// NOTE:
// This header defines the Empty protocol type and its msgpack serialization.
// Empty is mapped to msgpack::nil and is used to replace `void` in RPC interfaces.

namespace zlc
{

// Represents an explicit empty message in the protocol layer.
struct Empty
{
};

// A canonical Empty instance for request usage.
[[maybe_unused]] inline static Empty empty{};

// Encode an object into a msgpack byte buffer.
template <typename T> inline void encode(const T &obj, ByteBuffer &out)
{
  try
  {
    out.size = 0;
    msgpack::packer<ByteBuffer> pk(out);
    pk.pack(obj);
  }
  catch (const std::exception &e)
  {
    throw EncodeException(e.what());
  }
}

// Decode an object from a msgpack byte buffer.
template <typename T> inline void decode(const ByteView &bv, T &out)
{
  try
  {
    msgpack::object_handle oh =
        msgpack::unpack(reinterpret_cast<const char *>(bv.data), bv.size);
    oh.get().convert(out);
  }
  catch (const std::exception &e)
  {
    throw DecodeException(e.what());
  }
}

} // namespace zlc

// ============================================================================
// msgpack adaptors for zlc::Empty
//
// These adaptors tell msgpack how to serialize and deserialize zlc::Empty.
// Empty is encoded as msgpack::nil and decoded from msgpack::nil.
// ============================================================================

namespace msgpack
{
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
{
  namespace adaptor
  {

  // Serialize zlc::Empty as msgpack::nil.
  template <> struct pack<zlc::Empty>
  {
    template <typename Stream>
    msgpack::packer<Stream> &operator()(msgpack::packer<Stream> &o,
                                        const zlc::Empty &) const
    {
      o.pack_nil();
      return o;
    }
  };

  // Deserialize msgpack::nil into zlc::Empty.
  template <> struct convert<zlc::Empty>
  {
    const msgpack::object &operator()(const msgpack::object &o, zlc::Empty &) const
    {
      if (o.type != msgpack::type::NIL)
      {
        throw msgpack::type_error();
      }
      return o;
    }
  };

  } // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE
} // namespace msgpack
