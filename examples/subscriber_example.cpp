#include <iostream>
#include <string>

#include "lancom/lancom.hpp"

using namespace lancom;

// Callback function for received messages
void message_callback(const std::string& message) {
    std::cout << "Received: " << message << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    IPAddress node_ip = "127.0.0.1";
    if (argc > 1) {
        node_ip = argv[1];
    }
    
    std::cout << "Starting subscriber node on " << node_ip << std::endl;
    
    try {
        // Initialize node
        auto node = init_node("SubscriberNode", node_ip);
        
        // Create subscriber
        Subscriber subscriber(
            "example_topic",
            utils::string_decoder,
            message_callback);
        
        std::cout << "Subscriber started. Press Ctrl+C to exit." << std::endl;
        
        // Keep the node running
        node->spin();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
