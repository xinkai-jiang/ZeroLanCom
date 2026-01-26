#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <vector>

#include "zerolancom/nodes/heartbeat_message.hpp"
#include "zerolancom/nodes/node_info_manager.hpp"
#include "zerolancom/utils/periodic_task.hpp"
#include "zerolancom/utils/thread_pool.hpp"

namespace zlc
{

class MulticastSender : public Singleton<MulticastSender>
{
public:
  MulticastSender(const std::string &group, int port, const std::string &localIP,
                  const std::string &groupName);
  ~MulticastSender();

  void start();
  void stop();

private:
  void sendHeartbeat(const Bytes &msg);
  int sock_;
  sockaddr_in addr_{};
  std::unique_ptr<PeriodicTask> heartbeat_task_;
  NodeInfoManager *nodeInfoManager_;
  std::string groupName_;
};

class MulticastReceiver : public Singleton<MulticastReceiver>
{
public:
  MulticastReceiver(const std::string &group, int port, const std::string &localIP,
                    const std::string &groupName);
  ~MulticastReceiver();

  void start();
  void stop();

private:
  int sock_;
  std::string localIP_;
  std::string groupName_;
  std::unique_ptr<PeriodicTask> receive_task_;
  NodeInfoManager *nodeInfoManager_;
};

} // namespace zlc
