#include "lancom_node.hpp"
#include "utils/logger.hpp"

int main() {
    //initialize logger
    lancom::Logger::init(false); //true to enable file logging
    lancom::Logger::setLevel(lancom::LogLevel::INFO);
    lancom::LanComNode node("Node1", "127.0.0.1");
    node.spin();
    return 0;
}