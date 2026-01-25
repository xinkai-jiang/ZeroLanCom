#pragma once
#include "zerolancom/utils/singleton.hpp"
#include <mutex>
#include <string>
#include <zmq.hpp>

namespace zlc
{

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