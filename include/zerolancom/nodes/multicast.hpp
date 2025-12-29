#pragma once

#include <atomic>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>

#include "nodes/node_info.hpp"
#include "nodes/node_info_manager.hpp"

namespace zlc
{

class MulticastSender
{
public:
  MulticastSender(const std::string &group, int port, const std::string &localIP);
  ~MulticastSender();

  void start(const LocalNodeInfo &localInfo);
  void stop();

private:
  void sendHeartbeat(const std::vector<uint8_t> &msg);

private:
  int sock_;
  sockaddr_in addr_{};

  std::thread multicastSendThread_;
  std::atomic<bool> running_{false};
};

class MulticastReceiver
{
public:
  MulticastReceiver(const std::string &group, int port, const std::string &localIP);
  ~MulticastReceiver();

  void start(NodeInfoManager &nodeManager);
  void stop();

private:
  int sock_;
  std::thread multicastReceiveThread_;
  std::atomic<bool> running_{false};
};

} // namespace zlc
