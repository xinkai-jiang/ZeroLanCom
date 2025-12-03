// #include <chrono>
// #include <iostream>
// #include <string>
// #include <thread>

// #include "lancom/lancom.hpp"

// using namespace lancom;
// using namespace std::chrono_literals;

// int main(int argc, char* argv[]) {
//     // Parse command line arguments
//     IPAddress node_ip = "127.0.0.1";
//     if (argc > 1) {
//         node_ip = argv[1];
//     }
    
//     std::cout << "Starting publisher node on " << node_ip << std::endl;
    
//     try {
//         // Initialize node
//         auto node = init_node("PublisherNode", node_ip);
        
//         // Create publisher
//         Publisher publisher("example_topic");
        
//         // Publish messages in a loop
//         int count = 0;
//         while (true) {
//             std::string message = "Hello from C++ publisher! Count: " + std::to_string(count++);
//             std::cout << "Publishing: " << message << std::endl;
            
//             publisher.publish_string(message);
            
//             std::this_thread::sleep_for(1s);
//         }
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }
    
//     return 0;
// }
