#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <vector>

#include "zerolancom/nodes/node_info.hpp"
#include "zerolancom/nodes/node_info_manager.hpp"
#include "zerolancom/utils/periodic_task.hpp"
#include "zerolancom/utils/thread_pool.hpp"

namespace zlc
{

class MulticastSender
{
public:
  MulticastSender(const std::string &group, int port, const std::string &localIP);
  ~MulticastSender();

  void start(const LocalNodeInfo &localInfo, ThreadPool &pool);
  void stop();

private:
  void sendHeartbeat(const Bytes &msg);

private:
  int sock_;
  sockaddr_in addr_{};

  std::unique_ptr<PeriodicTask> heartbeat_task_;
};

class MulticastReceiver
{
public:
  MulticastReceiver(const std::string &group, int port, const std::string &localIP);
  ~MulticastReceiver();

  void start(NodeInfoManager &nodeManager, ThreadPool &pool);
  void stop();

private:
  int sock_;
  std::unique_ptr<PeriodicTask> receive_task_;
};

} // namespace zlc
