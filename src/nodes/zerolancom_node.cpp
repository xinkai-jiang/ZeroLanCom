#include "zerolancom/nodes/zerolancom_node.hpp"

#include <stdexcept>

namespace zlc
{

ZeroLanComNode::ZeroLanComNode(const std::string &name, const std::string &ip,
                               const std::string &group, int groupPort)
{
  localInfo_ = new LocalNodeInfo(name, ip);
  nodesManager_ = new NodeInfoManager();
  threadPool_ = new ThreadPool(0);
  serviceManager_ = new ServiceManager(ip);
  subscriberManager_ = new SubscriberManager();
  mcastSender_ = new MulticastSender(group, groupPort, ip);
  mcastReceiver_ = new MulticastReceiver(group, groupPort, ip);
  threadPool_->start();
  mcastSender_->start();
  mcastReceiver_->start();
  serviceManager_->start();
  subscriberManager_->start();
  running = true;
}

ZeroLanComNode::~ZeroLanComNode()
{
  stop();
}

void ZeroLanComNode::stop()
{
  running = false;
  mcastSender_->stop();
  mcastReceiver_->stop();
  serviceManager_->stop();
  subscriberManager_->stop();
  threadPool_->stop();
  delete mcastSender_;
  delete mcastReceiver_;
  delete serviceManager_;
  delete subscriberManager_;
  delete threadPool_;
  delete nodesManager_;
  delete localInfo_;
}

bool ZeroLanComNode::isRunning() const
{
  return running;
}

const std::string &ZeroLanComNode::GetIP() const
{
  return localInfo_->nodeInfo.ip;
}

} // namespace zlc
