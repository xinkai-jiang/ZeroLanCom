#include "zerolancom/nodes/zerolancom_node.hpp"

#include <stdexcept>

namespace zlc
{

/* ================= lifecycle ================= */

void ZeroLanComNode::init(const std::string &name, const std::string &ip,
                          const std::string &group, int groupPort)
{
  if (instance_)
  {
    throw std::logic_error("ZeroLanComNode singleton already initialized");
  }
  instance_.reset(new ZeroLanComNode(name, ip, group, groupPort));
}

ZeroLanComNode::ZeroLanComNode(const std::string &name, const std::string &ip,
                               const std::string &group, int groupPort)
    : localInfo(name, ip), mcastSender(group, groupPort, ip),
      mcastReceiver(group, groupPort, ip), serviceManager(ip),
      subscriberManager(nodesManager), running(true)
{
  mcastSender.start(localInfo);
  mcastReceiver.start(nodesManager);
  serviceManager.start();
  subscriberManager.start();
}

ZeroLanComNode::~ZeroLanComNode()
{
  stop();
}

void ZeroLanComNode::stop()
{
  running = false;
  mcastSender.stop();
  mcastReceiver.stop();
  serviceManager.stop();
  subscriberManager.stop();
}

bool ZeroLanComNode::isRunning() const
{
  return running;
}

/* ================= API ================= */

void ZeroLanComNode::registerTopic(const std::string &topic_name, int port)
{
  localInfo.registerTopic(topic_name, static_cast<uint16_t>(port));
  zlc::info("Topic {} registered at port {}", topic_name, port);
}

const std::string &ZeroLanComNode::GetIP() const
{
  return localInfo.nodeInfo.ip;
}

} // namespace zlc
