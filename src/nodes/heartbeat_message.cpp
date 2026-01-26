#include "zerolancom/nodes/heartbeat_message.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

namespace zlc
{

// Fixed size of the heartbeat message header (before group_name)
constexpr size_t HEARTBEAT_FIXED_SIZE = 56;

Bytes HeartbeatMessage::encode() const
{
  Bytes buf;
  buf.reserve(HEARTBEAT_FIXED_SIZE + group_name.size());

  // Write zlc_version (3 x int32, network byte order)
  for (int i = 0; i < 3; ++i)
  {
    uint32_t val = htonl(static_cast<uint32_t>(zlc_version[i]));
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&val);
    buf.insert(buf.end(), ptr, ptr + 4);
  }

  // Write node_id (36 bytes fixed string)
  if (node_id.size() != 36)
  {
    throw std::runtime_error("node_id must be exactly 36 characters");
  }
  buf.insert(buf.end(), node_id.begin(), node_id.end());

  // Write info_id (int32, network byte order)
  {
    uint32_t val = htonl(static_cast<uint32_t>(info_id));
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&val);
    buf.insert(buf.end(), ptr, ptr + 4);
  }

  // Write service_port (int32, network byte order)
  {
    uint32_t val = htonl(static_cast<uint32_t>(service_port));
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&val);
    buf.insert(buf.end(), ptr, ptr + 4);
  }

  // Write group_name (variable length)
  buf.insert(buf.end(), group_name.begin(), group_name.end());

  return buf;
}

HeartbeatMessage HeartbeatMessage::decode(const uint8_t *data, size_t size)
{
  if (size < HEARTBEAT_FIXED_SIZE)
  {
    throw std::runtime_error(
        "HeartbeatMessage: data too short, expected at least 56 bytes");
  }

  HeartbeatMessage msg;
  size_t offset = 0;

  // Read zlc_version (3 x int32, network byte order)
  for (int i = 0; i < 3; ++i)
  {
    uint32_t val;
    std::memcpy(&val, data + offset, 4);
    msg.zlc_version[i] = static_cast<int32_t>(ntohl(val));
    offset += 4;
  }

  // Read node_id (36 bytes fixed string)
  msg.node_id = std::string(reinterpret_cast<const char *>(data + offset), 36);
  offset += 36;

  // Read info_id (int32, network byte order)
  {
    uint32_t val;
    std::memcpy(&val, data + offset, 4);
    msg.info_id = static_cast<int32_t>(ntohl(val));
    offset += 4;
  }

  // Read service_port (int32, network byte order)
  {
    uint32_t val;
    std::memcpy(&val, data + offset, 4);
    msg.service_port = static_cast<int32_t>(ntohl(val));
    offset += 4;
  }

  // Read group_name (remaining bytes)
  if (size > HEARTBEAT_FIXED_SIZE)
  {
    msg.group_name =
        std::string(reinterpret_cast<const char *>(data + offset), size - offset);
  }

  return msg;
}

} // namespace zlc
