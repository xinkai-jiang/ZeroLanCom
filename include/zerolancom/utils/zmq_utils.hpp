#pragma once
#include "zerolancom/utils/singleton.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <mutex>
#include <string>
#include <zmq.hpp>

namespace zlc
{

// Default subnet mask for /24 network
constexpr uint32_t DEFAULT_SUBNET_MASK = 0xFFFFFF00; // 255.255.255.0

/**
 * @brief Check if two IP addresses are in the same subnet.
 *
 * @param ip1 First IP address (e.g., local IP)
 * @param ip2 Second IP address (e.g., remote IP)
 * @param subnetMask Subnet mask in host byte order (default: 255.255.255.0)
 * @return true if both IPs share the same network prefix
 */
inline bool isInSameSubnet(const std::string &ip1, const std::string &ip2,
                           uint32_t subnetMask = DEFAULT_SUBNET_MASK)
{
  in_addr addr1{}, addr2{};
  if (inet_aton(ip1.c_str(), &addr1) == 0)
    return false;
  if (inet_aton(ip2.c_str(), &addr2) == 0)
    return false;

  uint32_t ip1_host = ntohl(addr1.s_addr);
  uint32_t ip2_host = ntohl(addr2.s_addr);

  return (ip1_host & subnetMask) == (ip2_host & subnetMask);
}

using ZMQSocket = zmq::socket_t;

class ZMQContext : public Singleton<ZMQContext>
{
public:
  ZMQContext() : context_(1)
  {
  }
  static ZMQSocket *createSocket(zmq::socket_type type)
  {
    assert(instance_ != nullptr);
    return instance_->_createSocket(type);
  }

  static ZMQSocket createTempSocket(zmq::socket_type type)
  {
    assert(instance_ != nullptr);
    return ZMQSocket(instance_->context_, type);
  }
  ~ZMQContext()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &socket : sockets_)
    {
      socket->close();
    }
    context_.close();
  }

private:
  ZMQSocket *_createSocket(zmq::socket_type type)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    sockets_.emplace_back(std::make_unique<ZMQSocket>(context_, type));
    return sockets_.back().get();
  }

  std::mutex mutex_;
  zmq::context_t context_;
  std::vector<std::unique_ptr<ZMQSocket>> sockets_;
};

inline int getBoundPort(ZMQSocket &socket)
{
  // fetch endpoint string using modern cppzmq API
  std::string endpoint = socket.get(zmq::sockopt::last_endpoint);

  auto pos = endpoint.rfind(':');
  if (pos == std::string::npos)
  {
    throw std::runtime_error("Invalid endpoint: " + endpoint);
  }

  return std::stoi(endpoint.substr(pos + 1));
}

} // namespace zlc