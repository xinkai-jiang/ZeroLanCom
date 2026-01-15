#pragma once

#include <mutex>
#include <string>

#include <zmq.hpp>

#include "zerolancom/utils/logger.hpp"
#include "zerolancom/utils/singleton.hpp"
#include "zerolancom/utils/thread_pool.hpp"

#include "zerolancom/nodes/multicast.hpp"
#include "zerolancom/nodes/node_info.hpp"
#include "zerolancom/nodes/node_info_manager.hpp"
#include "zerolancom/sockets/service_manager.hpp"
#include "zerolancom/sockets/subscriber_manager.hpp"

namespace zlc
{

class ZeroLanComNode : public Singleton<ZeroLanComNode>
{
public:
  ~ZeroLanComNode();
  static void init(const std::string &name, const std::string &ip,
                   const std::string &group, int groupPort);

  void stop();
  bool isRunning() const;

  void registerTopic(const std::string &topic_name, int port);
  const std::string &GetIP() const;

  ThreadPool &getThreadPool()
  {
    return threadPool;
  }

  zmq::context_t &getZmqContext()
  {
    return zmq_context_;
  }

  LocalNodeInfo localInfo;
  NodeInfoManager nodesManager;
  ThreadPool threadPool;

private:
  ZeroLanComNode(const std::string &name, const std::string &ip,
                 const std::string &group, int groupPort);

  // ZMQ context must be declared BEFORE ServiceManager and SubscriberManager
  // because members are initialized in declaration order
  zmq::context_t zmq_context_;

public:
  ServiceManager serviceManager;
  SubscriberManager subscriberManager;

private:
  MulticastSender mcastSender;
  MulticastReceiver mcastReceiver;
  bool running;
};

} // namespace zlc
