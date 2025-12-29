#include "nodes/node_info.hpp"

#include <cstring>

#include "utils/logger.hpp"

namespace zlc
{

/* ================= SocketInfo ================= */

void SocketInfo::encode(BinWriter &w) const
{
  w.write_string(name);
  w.write_u16(port);
}

SocketInfo SocketInfo::decode(BinReader &r, const std::string &ip)
{
  SocketInfo si;
  si.name = r.read_string();
  si.ip = ip;
  si.port = r.read_u16();
  return si;
}

/* ================= NodeInfo ================= */

std::vector<uint8_t> NodeInfo::encode() const
{
  BinWriter w;

  w.write_fixed_string(nodeID, 36);
  w.write_u32(infoID);
  w.write_string(name);
  w.write_string(ip);

  w.write_u16(static_cast<uint16_t>(topics.size()));
  for (const auto &t : topics)
    t.encode(w);

  w.write_u16(static_cast<uint16_t>(services.size()));
  for (const auto &s : services)
    s.encode(w);

  return std::move(w.buf);
}

NodeInfo NodeInfo::decode(ByteView bv)
{
  BinReader r{bv};
  NodeInfo ni;

  ni.nodeID = r.read_fixed_string(36);
  ni.infoID = r.read_u32();
  ni.name = r.read_string();
  ni.ip = r.read_string();

  uint16_t topics_count = r.read_u16();
  ni.topics.reserve(topics_count);
  for (uint16_t i = 0; i < topics_count; ++i)
    ni.topics.push_back(SocketInfo::decode(r, ni.ip));

  uint16_t srv_count = r.read_u16();
  ni.services.reserve(srv_count);
  for (uint16_t i = 0; i < srv_count; ++i)
    ni.services.push_back(SocketInfo::decode(r, ni.ip));

  return ni;
}

void NodeInfo::printNodeInfo() const
{
  zlc::info("-------------------------------------------------------------");
  zlc::info("NodeID: {}", nodeID);
  zlc::info("InfoID: {}", infoID);
  zlc::info("Name: {}", name);
  zlc::info("IP: {}", ip);
  zlc::info("Topics:");
  for (const auto &t : topics)
  {
    zlc::info("  - {}:{}", t.name, t.port);
  }
  zlc::info("Services:");
  for (const auto &s : services)
  {
    zlc::info("  - {}:{}", s.name, s.port);
  }
}

/* ================= LocalNodeInfo ================= */

LocalNodeInfo::LocalNodeInfo(const std::string &name, const std::string &ip)
{
  nodeID = generateUUID();
  nodeInfo.nodeID = nodeID;
  nodeInfo.infoID = 0;
  nodeInfo.name = name;
  nodeInfo.ip = ip;
}

std::vector<uint8_t> LocalNodeInfo::createHeartbeat() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return nodeInfo.encode();
}

void LocalNodeInfo::registerTopic(const std::string &name, uint16_t port)
{
  std::lock_guard<std::mutex> lock(mutex_);
  nodeInfo.topics.push_back(SocketInfo{name, nodeInfo.ip, port});
  ++nodeInfo.infoID;
}

void LocalNodeInfo::registerServices(const std::string &name, uint16_t port)
{
  std::lock_guard<std::mutex> lock(mutex_);
  nodeInfo.services.push_back(SocketInfo{name, nodeInfo.ip, port});
  ++nodeInfo.infoID;
}

} // namespace zlc
