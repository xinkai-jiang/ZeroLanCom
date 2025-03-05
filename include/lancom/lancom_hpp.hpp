#pragma once

// Core headers
#include "lancom/config.hpp"
#include "lancom/log.hpp"
#include "lancom/types.hpp"

// Node implementations
#include "lancom/nodes/abstract_node.hpp"
#include "lancom/nodes/lancom_node.hpp"
#include "lancom/nodes/silent_node.hpp"

// Socket implementations
#include "lancom/sockets/publisher.hpp"
#include "lancom/sockets/subscriber.hpp"
#include "lancom/sockets/service.hpp"
#include "lancom/sockets/service_proxy.hpp"
#include "lancom/sockets/streamer.hpp"

// Utility functions
#include "lancom/utils/message_utils.hpp"
#include "lancom/utils/node_utils.hpp"

// Convenience functions
namespace lancom {

// Initialize a node
inline std::shared_ptr<LanComNode> init_node(
    const std::string& node_name,
    const IPAddress& node_ip) {
    return LanComNode::create(node_name, node_ip);
}

// Get library version
inline std::string version() {
    return Version::to_string();
}

} // namespace lancom
