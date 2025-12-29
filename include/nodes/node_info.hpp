#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "serialization/serializer.hpp"
#include "utils/message.hpp"

namespace zlc
{

/* ================= SocketInfo ================= */

struct SocketInfo
{
  std::string name;
  std::string ip;
  uint16_t port;

  void encode(BinWriter &w) const;
  static SocketInfo decode(BinReader &r, const std::string &ip);
};

/* ================= NodeInfo ================= */

struct NodeInfo
{
  std::string nodeID; // 36-char UUID
  uint32_t infoID;
  std::string name;
  std::string ip;
  std::vector<SocketInfo> topics;
  std::vector<SocketInfo> services;

  std::vector<uint8_t> encode() const;
  static NodeInfo decode(ByteView bv);
  void printNodeInfo() const;
};

/* ================= LocalNodeInfo ================= */

struct LocalNodeInfo
{
  std::string nodeID;
  NodeInfo nodeInfo;
  mutable std::mutex mutex_;

  LocalNodeInfo(const std::string &name, const std::string &ip);

  std::vector<uint8_t> createHeartbeat() const;
  void registerTopic(const std::string &name, uint16_t port);
  void registerServices(const std::string &name, uint16_t port);
};

} // namespace zlc
