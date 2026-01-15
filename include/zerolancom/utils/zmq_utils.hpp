#pragma once
#include <string>
#include <zmq.hpp>

inline int getBoundPort(zmq::socket_t &socket)
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
