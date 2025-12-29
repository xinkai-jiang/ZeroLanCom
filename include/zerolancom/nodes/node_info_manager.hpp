#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "nodes/node_info.hpp"

namespace zlc
{

class NodeInfoManager
{
private:
  // data
  mutable std::shared_mutex data_mutex_;
  std::unordered_map<std::string, NodeInfo> nodes_info_;
  std::unordered_map<std::string, uint32_t> nodes_info_id_;
  std::unordered_map<std::string, std::chrono::steady_clock::time_point>
      nodes_heartbeat_;

  // callbacks
  mutable std::shared_mutex callback_mutex_;
  std::vector<std::function<void(const NodeInfo &)>> update_callback_;

private:
  // internal helpers (require external locking)
  void updateNodeUnlocked(const std::string &nodeID, const NodeInfo &info);
  bool checkNodeIDUnlocked(const std::string &nodeID) const;
  bool checkNodeInfoIDUnlocked(const std::string &nodeID, uint32_t infoID) const;

public:
  void registerUpdateCallback(const std::function<void(const NodeInfo &)> &callback);

  bool checkNodeID(const std::string &nodeID) const;
  bool checkNodeInfoID(const std::string &nodeID, uint32_t infoID) const;

  void removeNode(const std::string &nodeID);

  std::vector<SocketInfo> getPublisherInfo(const std::string &topicName) const;
  const SocketInfo *getServiceInfo(const std::string &serviceName) const;

  void checkHeartbeats();
  void processHeartbeat(const NodeInfo &nodeInfo);
};

} // namespace zlc
