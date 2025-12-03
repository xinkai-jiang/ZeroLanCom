// #pragma once

// #include <memory>
// #include <string>
// #include <unordered_map>

// #include "lancom/types.hpp"

// namespace lancom {

// // Forward declaration
// class LanComNode;

// // Base class for socket implementations
// class AbstractLanComSocket {
// public:
//     AbstractLanComSocket(
//         const std::string& name,
//         ComponentTypeEnum component_type,
//         bool with_local_namespace = true);
    
//     virtual ~AbstractLanComSocket() = default;
    
//     // Non-copyable
//     AbstractLanComSocket(const AbstractLanComSocket&) = delete;
//     AbstractLanComSocket& operator=(const AbstractLanComSocket&) = delete;
    
//     // Shutdown the socket
//     void shutdown();
    
//     // Get socket info
//     const SocketInfo& get_info() const;

// protected:
//     // Set up the socket with ZMQ
//     void set_up_socket(zmq::socket_t& zmq_socket);
    
//     // Shutdown implementation
//     virtual void on_shutdown() = 0;
    
//     // Socket properties
//     std::string name_;
//     SocketInfo info_;
//     bool running_;
    
//     // Reference to parent node
//     std::shared_ptr<LanComNode> node_;
// };

// // Publisher socket implementation
// class Publisher : public AbstractLanComSocket {
// public:
//     // Create a new publisher
//     explicit Publisher(
//         const std::string& topic_name,
//         bool with_local_namespace = false);
    
//     // Destructor
//     ~Publisher() override = default;
    
//     // Publish methods for different types
//     void publish_bytes(const BytesMessage& bytes_msg);
//     void publish_string(const std::string& msg);
//     void publish_dict(const std::unordered_map<std::string, std::string>& data);
    
// protected:
//     // Implement shutdown
//     void on_shutdown() override;
    
//     // Asynchronous send
//     Task<void> send_bytes_async(const BytesMessage& bytes_msg);
    
//     // Socket reference
//     zmq::socket_t& socket_;
// };

// } // namespace lancom
