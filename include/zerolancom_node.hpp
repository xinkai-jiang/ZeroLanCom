#pragma once
#include <string>
#include <chrono>
#include <thread>
#include "nodes/node_info_manager.hpp"
#include "nodes/node_info.hpp"
#include "nodes/multicast.hpp"
#include "sockets/service_manager.hpp"
#include "sockets/subscriber_manager.hpp"
#include "utils/singleton.hpp"

namespace zlc {

class ZeroLanComNode : public Singleton<ZeroLanComNode> {
public:

static void init(
    const std::string& name,
     const std::string& ip,
      const zlc::LogLevel log_level = zlc::LogLevel::INFO) 
{
    Logger::init(false);
    Logger::setLevel(log_level);
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_) {
            throw std::logic_error("Singleton already initialized");
        }
        instance_.reset(new ZeroLanComNode(name, ip));
}

ZeroLanComNode(const std::string& name,
    const std::string& ip,
    const std::string& group="224.0.0.1",
    int groupPort=7720)
    : localInfo(name, ip),
    mcastSender(group, groupPort, ip),
    mcastReceiver(group, groupPort, ip),
    serviceManager(ip),
    subscriberManager(nodesManager)
    {
        mcastSender.start(localInfo);
        mcastReceiver.start(nodesManager);
        serviceManager.start();
        subscriberManager.start();
    }

    ~ZeroLanComNode() {
        stop();
    }

    void stop() {
        running = false;
        mcastSender.stop();
        mcastReceiver.stop();
    }

    void registerTopic(const std::string& topic_name, int port)
    {
        localInfo.registerTopic(topic_name, static_cast<uint16_t>(port));
        LOG_INFO("Topic {} registered at port {}", topic_name, port);
    }

    const std::string& GetIP() const {
        return localInfo.nodeInfo.ip;
    }

    NodeInfoManager nodesManager;
    bool isRunning() const {
        return running;
    }

    LocalNodeInfo localInfo;
    ServiceManager serviceManager;
    SubscriberManager subscriberManager;

private:
MulticastSender mcastSender;
MulticastReceiver mcastReceiver;

bool running = true;
};

} // namespace zlc