#pragma once

#include <mutex>
#include <string>

#include "zerolancom/utils/logger.hpp"
#include "zerolancom/utils/singleton.hpp"

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

  NodeInfoManager nodesManager;
  ServiceManager serviceManager;
  SubscriberManager subscriberManager;
  LocalNodeInfo localInfo;

private:
  ZeroLanComNode(const std::string &name, const std::string &ip,
                 const std::string &group, int groupPort);
  MulticastSender mcastSender;
  MulticastReceiver mcastReceiver;
  bool running;
};

} // namespace zlc
