#include "zerolancom/nodes/zerolancom_node.hpp"

#include <stdexcept>

namespace zlc
{

// Default group name
constexpr const char *DEFAULT_GROUP_NAME = "zlc_default";

ZeroLanComNode::ZeroLanComNode(const std::string &name, const std::string &ip,
                               const std::string &group, int groupPort)
    : ZeroLanComNode(name, ip, group, groupPort, DEFAULT_GROUP_NAME)
{
}

ZeroLanComNode::ZeroLanComNode(const std::string &name, const std::string &ip,
                               const std::string &group, int groupPort,
                               const std::string &groupName)
{
  ThreadPool::initExternal(4); // 4 worker threads
  ZMQContext::initExternal();
  NodeInfoManager::initExternal(name, ip);
  ServiceManager::initExternal(ip);

  // Set service port in NodeInfoManager before starting multicast
  NodeInfoManager::instance().setServicePort(ServiceManager::instance().service_port);

  MulticastReceiver::initExternal(group, groupPort, ip, groupName);
  MulticastSender::initExternal(group, groupPort, ip, groupName);
  SubscriberManager::initExternal();

  // Register internal get_node_info service
  registerGetNodeInfoService();

  ThreadPool::instance().start();
  MulticastSender::instance().start();
  MulticastReceiver::instance().start();
  ServiceManager::instance().start();
  SubscriberManager::instance().start();
  running = true;
}

ZeroLanComNode::~ZeroLanComNode()
{
  stop();
}

void ZeroLanComNode::registerGetNodeInfoService()
{
  auto &serviceManager = ServiceManager::instance();
  serviceManager.registerHandler<Empty, NodeInfo>(
      "get_node_info",
      [](const Empty &) -> NodeInfo
      { return NodeInfoManager::instance().getLocalNodeInfo(); });
}

void ZeroLanComNode::stop()
{
  running = false;

  MulticastSender::instance().stop();
  MulticastReceiver::instance().stop();
  ServiceManager::instance().stop();
  SubscriberManager::instance().stop();
  ThreadPool::instance().stop();

  ThreadPool::destroy();
  ZMQContext::destroy();
  NodeInfoManager::destroy();
  MulticastSender::destroy();
  MulticastReceiver::destroy();
  ServiceManager::destroy();
  SubscriberManager::destroy();
}

bool ZeroLanComNode::isRunning() const
{
  return running;
}

} // namespace zlc
