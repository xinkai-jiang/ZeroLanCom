#include <iostream>
#include <string>

#include "lancom/lancom.hpp"

using namespace lancom;

// Service handler function
std::string echo_service(const std::string& request) {
    std::cout << "Service received: " << request << std::endl;
    return "Echo: " + request;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    IPAddress node_ip = "127.0.0.1";
    if (argc > 1) {
        node_ip = argv[1];
    }
    
    std::cout << "Starting service node on " << node_ip << std::endl;
    
    try {
        // Initialize node
        auto node = init_node("ServiceNode", node_ip);
        
        // Create service
        Service service(
            "echo_service",
            utils::string_decoder,
            utils::string_encoder,
            echo_service);
        
        std::cout << "Service started. Press Ctrl+C to exit." << std::endl;
        
        // Keep the node running
        node->spin();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
