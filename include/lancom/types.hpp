#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace lancom {

// Basic type definitions
using IPAddress = std::string;
using Port = uint16_t;
using TopicName = std::string;
using ServiceName = std::string;
using HashIdentifier = std::string;
using ComponentType = std::string;
using TimePoint = std::chrono::steady_clock::time_point;

// Message type definitions
using BytesMessage = std::vector<uint8_t>;
using StringMessage = std::string;

// Variants for message types
using Message = std::variant<BytesMessage, StringMessage>;

// Response status
enum class ResponseStatus {
    SUCCESS,
    ERROR,
    TIMEOUT,
    EMPTY
};

// Socket information
struct SocketInfo {
    std::string name;
    HashIdentifier socketID;
    HashIdentifier nodeID;
    ComponentType type;
    IPAddress ip;
    Port port;
};

// Node information
struct NodeInfo {
    std::string name;
    HashIdentifier nodeID;
    uint32_t infoID;
    IPAddress ip;
    std::string type;
    Port port;
    std::vector<SocketInfo> publishers;
    std::vector<SocketInfo> services;
};

// Component types
enum class ComponentTypeEnum {
    PUBLISHER,
    SUBSCRIBER,
    SERVICE
};

// Convert ComponentTypeEnum to string
inline std::string component_type_to_string(ComponentTypeEnum type) {
    switch (type) {
        case ComponentTypeEnum::PUBLISHER:
            return "publisher";
        case ComponentTypeEnum::SUBSCRIBER:
            return "subscriber";
        case ComponentTypeEnum::SERVICE:
            return "service";
        default:
            return "unknown";
    }
}

// Node request types
enum class NodeReqType {
    PING,
    NODE_INFO
};

// Convert NodeReqType to string
inline std::string node_req_type_to_string(NodeReqType type) {
    switch (type) {
        case NodeReqType::PING:
            return "PING";
        case NodeReqType::NODE_INFO:
            return "NODE_INFO";
        default:
            return "UNKNOWN";
    }
}

// Message types for serialization
enum class MsgType {
    BYTES,
    STRING,
    JSON,
    MSGPACK
};

// LanCom standard messages
enum class LanComMsg {
    SUCCESS,
    ERROR,
    TIMEOUT,
    EMPTY
};

// Convert LanComMsg to string
inline std::string lancom_msg_to_string(LanComMsg msg) {
    switch (msg) {
        case LanComMsg::SUCCESS:
            return "SUCCESS";
        case LanComMsg::ERROR:
            return "ERROR";
        case LanComMsg::TIMEOUT:
            return "TIMEOUT";
        case LanComMsg::EMPTY:
            return "EMPTY";
        default:
            return "UNKNOWN";
    }
}

} // namespace lancom
