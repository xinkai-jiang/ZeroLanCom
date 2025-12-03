#pragma once
#include "nodes/node_info_manager.hpp"
#include "nodes/node_info.hpp"
#include "nodes/multicast.hpp"
// #include "sockets/service_manager.hpp"
// #include "sockets/zmq_request_helper.hpp"
#include <string>
#include <thread>
#include <chrono>

namespace lancom {

class LanComNode {
public:

LanComNode(const std::string& name,
    const std::string& ip,
    const std::string& group="224.0.0.1",
    int groupPort=7720)
    : localInfo(name, ip),
    mcastSender(group, groupPort, ip),
    mcastReceiver(group, groupPort, ip)
    // serviceManager(localInfo.port)
    {
        mcastSender.start(localInfo);
        mcastReceiver.start(nodesManager);
    }

    ~LanComNode() {
        stop();
    }

    void stop() {
        running = false;
        mcastSender.stop();
        mcastReceiver.stop();
    }

    void spin() {
        try
        {
            while (running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

private:
    
    LocalNodeInfo localInfo;
    NodeInfoManager nodesManager;
    
    MulticastSender mcastSender;
    MulticastReceiver mcastReceiver;
    
    // ServiceManager serviceManager;

    std::thread multicastSendThread;
    std::thread multicastRecvThread;
    std::thread serviceThread;
    
    bool running = true;
    int startZmqServicePort() { return 7000; }

};

} // namespace lancom