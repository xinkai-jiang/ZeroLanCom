#include "zerolancom.hpp"

void topicCallback(const std::string& msg) {
    LOG_INFO("Received message on subscribed topic: {}", msg);
}

std::string serviceHandler(const std::string& request) {
    LOG_INFO("Service request received.");
    return std::string("Echo: ") + request;
}

int main() {
    //initialize logger
    zlc::ZeroLanComNode::init("TestNode", "127.0.0.1");
    zlc::registerServiceHandler("EchoService", serviceHandler);
    zlc::registerSubscriberHandler("TestTopic", topicCallback);
    zlc::Publisher<std::string> publisher("TestTopic");
    zlc::registerServiceHandler("EchoService2", serviceHandler);
    zlc::Client::waitForService("EchoService");
    std::string response = "";
    zlc::request("EchoService", "Hello Service", response);
    try
    {
        while (true) {
            publisher.publish("Hello, ZeroLanCom!");
            LOG_INFO("Published message to TestTopic");
            zlc::sleep(1000);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}