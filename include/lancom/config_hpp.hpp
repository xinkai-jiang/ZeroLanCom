#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace lancom {

// Version information
struct Version {
    static constexpr uint8_t MAJOR = 1;
    static constexpr uint8_t MINOR = 0;
    static constexpr uint8_t PATCH = 0;
    
    static constexpr std::array<uint8_t, 3> VERSION_BYTES = {MAJOR, MINOR, PATCH};
    static constexpr std::array<uint8_t, 2> COMPATIBILITY = {MAJOR, MINOR};
    
    static std::string to_string() {
        return std::to_string(MAJOR) + "." + 
               std::to_string(MINOR) + "." + 
               std::to_string(PATCH);
    }
};

// Network configuration
constexpr std::string_view MULTICAST_ADDR = "224.0.0.1";
constexpr uint16_t MULTICAST_PORT = 7720;
constexpr uint16_t MASTER_SERVICE_PORT = 7721;

// Timing configuration
constexpr std::chrono::milliseconds BROADCAST_INTERVAL{500}; // 0.5 seconds
constexpr std::chrono::milliseconds HEARTBEAT_INTERVAL{200}; // 0.2 seconds
constexpr std::chrono::seconds NODE_TIMEOUT{5};             // 5 seconds

// Protocol constants
constexpr std::string_view PROTOCOL_HEADER = "LANCOM";

// Helper functions
BytesMessage create_version_bytes();
BytesMessage create_compatibility_bytes();

} // namespace lancom
