#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "zerolancom/nodes/node_info.hpp"
#include "zerolancom/utils/event.hpp"

namespace zlc
{

class NodeInfoManager : public Singleton<NodeInfoManager>
{
private:
  // data
  mutable std::shared_mutex data_mutex_;
  std::unordered_map<std::string, NodeInfo> nodes_info_;
  std::unordered_map<std::string, uint32_t> nodes_info_id_;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      nodes_heartbeat_;

  // internal helpers (require external locking)
  void updateNodeUnlocked(const std::string &nodeID, const NodeInfo &info);
  bool checkNodeIDUnlocked(const std::string &nodeID) const;
  bool checkNodeInfoIDUnlocked(const std::string &nodeID, uint32_t infoID) const;

public:
  // event for node updates
  Event<const NodeInfo &> node_update_event;
  Event<const NodeInfo &> node_remove_event;

  bool checkNodeID(const std::string &nodeID) const;
  bool checkNodeInfoID(const std::string &nodeID, uint32_t infoID) const;
  void removeNode(const std::string &nodeID);

  std::vector<SocketInfo> getPublisherInfo(const std::string &topicName) const;
  const SocketInfo *getServiceInfo(const std::string &serviceName) const;

  void checkHeartbeats();
  void processHeartbeat(const NodeInfo &nodeInfo);
};

} // namespace zlc
