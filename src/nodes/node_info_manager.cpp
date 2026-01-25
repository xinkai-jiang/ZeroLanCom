#include "zerolancom/nodes/node_info_manager.hpp"

#include <iostream>

#include "zerolancom/utils/logger.hpp"

namespace zlc
{

/* ================= private helpers ================= */

void NodeInfoManager::updateNodeUnlocked(const std::string &nodeID,
                                         const NodeInfo &info)
{
  nodes_info_[nodeID] = info;
  nodes_info_id_[nodeID] = info.infoID;
  nodes_heartbeat_[nodeID] = std::chrono::steady_clock::now();
}

bool NodeInfoManager::checkNodeIDUnlocked(const std::string &nodeID) const
{
  return nodes_info_.find(nodeID) != nodes_info_.end();
}

bool NodeInfoManager::checkNodeInfoIDUnlocked(const std::string &nodeID,
                                              uint32_t infoID) const
{
  auto it = nodes_info_id_.find(nodeID);
  if (it == nodes_info_id_.end())
    return false;
  return it->second == infoID;
}

/* ================= public API ================= */

bool NodeInfoManager::checkNodeID(const std::string &nodeID) const
{
  std::shared_lock lock(data_mutex_);
  return checkNodeIDUnlocked(nodeID);
}

bool NodeInfoManager::checkNodeInfoID(const std::string &nodeID, uint32_t infoID) const
{
  std::shared_lock lock(data_mutex_);
  return checkNodeInfoIDUnlocked(nodeID, infoID);
}

void NodeInfoManager::removeNode(const std::string &nodeID)
{
  std::unique_lock lock(data_mutex_);
  nodes_info_.erase(nodeID);
  nodes_info_id_.erase(nodeID);
  nodes_heartbeat_.erase(nodeID);
}

std::vector<SocketInfo>
NodeInfoManager::getPublisherInfo(const std::string &topicName) const
{
  std::shared_lock lock(data_mutex_);

  std::vector<SocketInfo> result;
  for (const auto &[id, node] : nodes_info_)
  {
    for (const auto &t : node.topics)
    {
      if (t.name == topicName)
      {
        result.push_back(t);
      }
    }
  }
  return result;
}

const SocketInfo *NodeInfoManager::getServiceInfo(const std::string &serviceName) const
{
  std::shared_lock lock(data_mutex_);

  for (const auto &[id, node] : nodes_info_)
  {
    for (const auto &t : node.services)
    {
      if (t.name == serviceName)
      {
        return &t;
      }
    }
  }
  return nullptr;
}

void NodeInfoManager::checkHeartbeats()
{
  std::unique_lock lock(data_mutex_);

  auto now = std::chrono::steady_clock::now();
  std::vector<std::string> to_remove;

  for (const auto &[nodeID, last] : nodes_heartbeat_)
  {
    auto duration =
        std::chrono::duration_cast<std::chrono::seconds>(now - last).count();
    if (duration > 2)
    {
      to_remove.push_back(nodeID);
    }
  }

  for (const auto &nodeID : to_remove)
  {
    node_remove_event.trigger(nodes_info_[nodeID]);
    nodes_info_.erase(nodeID);
    nodes_info_id_.erase(nodeID);
    nodes_heartbeat_.erase(nodeID);
    zlc::info("Node {} removed due to heartbeat timeout", nodeID);
  }
}

void NodeInfoManager::processHeartbeat(const NodeInfo &nodeInfo)
{
  try
  {
    bool is_new = false;
    bool info_changed = false;

    {
      std::unique_lock lock(data_mutex_);

      if (!checkNodeIDUnlocked(nodeInfo.nodeID))
      {
        updateNodeUnlocked(nodeInfo.nodeID, nodeInfo);
        is_new = true;
      }
      else if (!checkNodeInfoIDUnlocked(nodeInfo.nodeID, nodeInfo.infoID))
      {
        updateNodeUnlocked(nodeInfo.nodeID, nodeInfo);
        info_changed = true;
      }
      else
      {
        nodes_heartbeat_[nodeInfo.nodeID] = std::chrono::steady_clock::now();
      }
    }

    if (is_new)
    {
      zlc::info("Node {} added via heartbeat", nodeInfo.name);
      nodeInfo.printNodeInfo();
    }

    if (is_new || info_changed)
    {
      node_update_event.trigger(nodeInfo);
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error processing heartbeat: " << e.what() << "\n";
  }
}

} // namespace zlc
