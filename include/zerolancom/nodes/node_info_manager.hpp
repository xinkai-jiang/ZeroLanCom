#pragma once

#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "zerolancom/nodes/heartbeat_message.hpp"
#include "zerolancom/nodes/node_info.hpp"
#include "zerolancom/utils/event.hpp"
#include "zerolancom/utils/singleton.hpp"

namespace zlc
{

class NodeInfoManager : public Singleton<NodeInfoManager>
{
private:
  // Remote nodes data
  mutable std::shared_mutex data_mutex_;
  std::unordered_map<std::string, NodeInfo> nodes_info_;
  std::unordered_map<std::string, uint32_t> nodes_info_id_;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      nodes_heartbeat_;

  // Local node data
  mutable std::mutex local_mutex_;
  NodeInfo localNodeInfo_;
  std::string groupName_;
  int32_t servicePort_{0};

  // internal helpers (require external locking)
  void updateNodeUnlocked(const std::string &nodeID, const NodeInfo &info);
  bool checkNodeIDUnlocked(const std::string &nodeID) const;
  bool checkNodeInfoIDUnlocked(const std::string &nodeID, uint32_t infoID) const;

  // Fetch full NodeInfo from remote node
  std::optional<NodeInfo> fetchNodeInfo(const std::string &ip, int32_t servicePort);

public:
  NodeInfoManager(const std::string &name, const std::string &ip);

  // event for node updates
  Event<const NodeInfo &> node_update_event;
  Event<const NodeInfo &> node_remove_event;

  // Remote node queries
  bool checkNodeID(const std::string &nodeID) const;
  bool checkNodeInfoID(const std::string &nodeID, uint32_t infoID) const;
  void removeNode(const std::string &nodeID);

  std::vector<SocketInfo> getPublisherInfo(const std::string &topicName) const;
  const SocketInfo *getServiceInfo(const std::string &serviceName) const;

  void checkHeartbeats();
  void processHeartbeat(const HeartbeatMessage &heartbeat, const std::string &nodeIP);

  // Local node management
  const UUID &nodeID() const;
  void setGroupName(const std::string &name);
  void setServicePort(int32_t port);
  HeartbeatMessage createHeartbeat() const;
  NodeInfo getLocalNodeInfo() const;
  void registerLocalTopic(const std::string &name, uint16_t port);
  void registerLocalService(const std::string &name, uint16_t port);
};

} // namespace zlc
