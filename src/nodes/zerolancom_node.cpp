#include "zerolancom/nodes/zerolancom_node.hpp"

#include <stdexcept>

namespace zlc
{

ZeroLanComNode::ZeroLanComNode(const std::string &name, const std::string &ip,
                               const std::string &group, int groupPort)
{
  ThreadPool::initExternal(4); // 4 worker threads
  ZMQContext::initExternal();
  LocalNodeInfo::initExternal(name, ip);
  NodeInfoManager::initExternal();
  MulticastReceiver::initExternal(group, groupPort, ip);
  MulticastSender::initExternal(group, groupPort, ip);
  ServiceManager::initExternal(ip);
  SubscriberManager::initExternal();

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
  LocalNodeInfo::destroy();
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
