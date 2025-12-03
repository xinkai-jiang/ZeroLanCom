#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "lancom.hpp"

using namespace lancom;
using namespace std::chrono_literals;

// Node task function
void start_node_task(const std::string& node_name, const IPAddress& node_ip) {
    try {
        std::cout << "Starting node " << node_name << " on " << node_ip << std::endl;
        
        // Initialize node
        auto node = init_node(node_name, node_ip);
        
        // Spin until interrupted
        node->spin();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in node " << node_name << ": " << e.what() << std::endl;
    }
}

int main() {
    std::vector<std::thread> node_threads;
    std::vector<IPAddress> node_ips = {"127.0.0.1", "127.0.0.1", "127.0.0.1"};
    
    // Start nodes in separate threads
    for (size_t i = 0; i < node_ips.size(); ++i) {
        std::string node_name = "Node_" + std::to_string(i);
        node_threads.emplace_back(start_node_task, node_name, node_ips[i]);
        std::this_thread::sleep_for(1s);  // Allow some time for the node to start
    }
    
    // Wait for all nodes to run for some time
    std::this_thread::sleep_for(10s);
    
    // Clean up
    std::cout << "Stopping all nodes..." << std::endl;
    
    // Send termination signal (Ctrl+C) to all threads
    for (auto& thread : node_threads) {
        if (thread.joinable()) {
            // In a real application, you would need a way to signal the threads to stop
            // Here we simply wait for them to join
            thread.join();
        }
    }
    
    std::cout << "All nodes have been started and terminated" << std::endl;
    
    return 0;
}
