#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "lancom/nodes/abstract_node.hpp"
#include "lancom/types.hpp"

namespace lancom {

// Callback type for service handlers
using ServiceCallback = std::function<BytesMessage(const BytesMessage&)>;

// Main node implementation
class LanComNode : public AbstractNode {
public:
    // Singleton instance
    static std::shared_ptr<LanComNode> instance;
    
    // Create a new LanComNode (or return existing instance)
    static std::shared_ptr<LanComNode> create(
        const std::string& node_name,
        const IPAddress& node_ip);
    
    // Destructor
    ~LanComNode() override;
    
    // Get node information
    const NodeInfo& get_node_info() const;
    
    // Register a service callback
    void register_service(const std::string& service_name, ServiceCallback callback);
    
    // Unregister a service
    void unregister_service(const std::string& service_name);
    
    // Access to sockets for use by publishers/subscribers/services
    zmq::socket_t& get_pub_socket();
    zmq::socket_t& get_service_socket();

protected:
    // Initialize the event loop
    void initialize_event_loop() override;
    
    // Loop for sending multicast announcements
    Task<void> multicast_loop();
    
    // Loop for handling service requests
    Task<void> service_loop(
        zmq::socket_t& service_socket,
        const std::map<std::string, ServiceCallback>& services);
    
    // Default callback for node info requests
    BytesMessage ping_cbs(const BytesMessage& request);

private:
    // Private constructor for singleton pattern
    LanComNode(const std::string& node_name, const IPAddress& node_ip);
    
    // Node ID and information
    HashIdentifier node_id_;
    NodeInfo local_info_;
    
    // Service callbacks
    std::map<std::string, ServiceCallback> service_cbs_;
    
    // Sockets
    std::unique_ptr<zmq::socket_t> pub_socket_;
    std::unique_ptr<zmq::socket_t> service_socket_;
};

// Initialize a node (convenience function)
std::shared_ptr<LanComNode> init_node(
    const std::string& node_name,
    const IPAddress& node_ip);

} // namespace lancom
