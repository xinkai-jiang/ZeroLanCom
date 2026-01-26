#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <msgpack.hpp>

#include "zerolancom/utils/message.hpp"

namespace zlc
{

/* ================= SocketInfo ================= */

struct SocketInfo
{
  std::string name;
  std::string ip;
  uint16_t port;

  MSGPACK_DEFINE(name, ip, port)
};

/* ================= NodeInfo ================= */

struct NodeInfo
{
  UUID nodeID; // 36-char UUID
  uint32_t infoID;
  std::string name;
  std::string ip;
  std::vector<SocketInfo> topics;
  std::vector<SocketInfo> services;

  MSGPACK_DEFINE(nodeID, infoID, name, ip, topics, services)

  void printNodeInfo() const;
};

} // namespace zlc
