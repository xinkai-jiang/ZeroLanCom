#include <msgpack.hpp>
#include "lancom_node.hpp"
#include "utils/logger.hpp"
#include "sockets/lancom_publisher.hpp"
#include "sockets/lancom_client.hpp"

void topicCallback(const std::string& msg) {
    LOG_INFO("Received message on subscribed topic: {}", msg);
}

struct CustomMessage
{
    int count;
    std::string name;
    std::vector<float> data;
    MSGPACK_DEFINE_MAP(count, name, data);
};



int main() {
    //initialize logger
    lancom::Logger::init(false); //true to enable file logging
    lancom::Logger::setLevel(lancom::LogLevel::INFO);
    lancom::LanComNode& node = lancom::LanComNode::init("CustomMessageNode", "127.0.0.1");
    lancom::LanComPublisher<CustomMessage> publisher("CustomMessage");
    try
    {
        int counter = 0;
        while (true) {
            publisher.publish(CustomMessage{counter, "CustomMessage", {1.0f, 2.0f, 3.0f}});
            counter++;
            LOG_INFO("Published message to CustomMessage");
            node.sleep(1000);
        }
    }   
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}