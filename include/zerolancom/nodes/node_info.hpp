#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "zerolancom/serialization/serializer.hpp"
#include "zerolancom/utils/message.hpp"
#include "zerolancom/utils/singleton.hpp"

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
  UUID nodeID; // 36-char UUID
  uint32_t infoID;
  std::string name;
  std::string ip;
  std::vector<SocketInfo> topics;
  std::vector<SocketInfo> services;

  Bytes encode() const;
  static NodeInfo decode(ByteView bv);
  void printNodeInfo() const;
};

/* ================= LocalNodeInfo ================= */

class LocalNodeInfo : public Singleton<LocalNodeInfo>
{
public:
  const UUID& nodeID;
  NodeInfo nodeInfo;
  
  LocalNodeInfo(const std::string &name, const std::string &ip);
  
  Bytes createHeartbeat() const;
  void registerTopic(const std::string &name, uint16_t port);
  void registerServices(const std::string &name, uint16_t port);

private:
  mutable std::mutex mutex_;
};

} // namespace zlc
