#pragma once

#include <string>
#include <vector>

#include <zmq.hpp>

#include "lancom/types.hpp"

namespace lancom {
namespace utils {

// Generate a unique hash identifier (UUID v4)
HashIdentifier create_hash_identifier();

// Create a SHA256 hash from a string
std::string create_sha256(const std::string& input);

// Create a heartbeat message for node discovery
BytesMessage create_heartbeat_message(
    const HashIdentifier& node_id,
    Port port,
    uint32_t info_id);

// Get the port from a ZMQ socket
Port get_socket_port(const zmq::socket_t& socket);

// Calculate broadcast address from IP
IPAddress calculate_broadcast_addr(const IPAddress& ip_addr);

// Asynchronous function to send a request to a node and receive a response
Task<BytesMessage> send_bytes_request(
    const std::string& addr,
    const std::string& service_name,
    const BytesMessage& bytes_msgs,
    double timeout = 1.0);

// Encode a message with a type identifier
BytesMessage encode_message(const Message& msg, MsgType type);

// Decode a message based on its type identifier
Message decode_message(const BytesMessage& data);

} // namespace utils
} // namespace lancom
