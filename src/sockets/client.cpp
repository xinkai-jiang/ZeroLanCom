#include "sockets/client.hpp"

#include <chrono>
#include <thread>

namespace zlc
{

void Client::connect(zmq::socket_t &socket, const std::string &service_name)
{
  auto &node = ZeroLanComNode::instance();

  // Lookup service info from node discovery table
  auto serviceInfoPtr = node.nodesManager.getServiceInfo(service_name);

  if (serviceInfoPtr == nullptr)
  {
    zlc::error("Service {} is not available", service_name);
    return;
  }

  const SocketInfo &serviceInfo = *serviceInfoPtr;

  zlc::info("[Client] Found service '{}' at {}:{}", service_name, serviceInfo.ip,
            serviceInfo.port);

  // Connect socket to service endpoint
  socket.connect("tcp://" + serviceInfo.ip + ":" + std::to_string(serviceInfo.port));

  zlc::info("[Client] Connected to service '{}' at {}:{}", service_name, serviceInfo.ip,
            serviceInfo.port);
}

void Client::sendRequest(const std::string &service_name, const ByteView &payload,
                         zmq::socket_t &socket)
{
  // Send service name frame
  socket.send(zmq::buffer(service_name), zmq::send_flags::sndmore);

  // Send payload frame
  socket.send(zmq::buffer(payload.data, payload.size), zmq::send_flags::none);

  zlc::info("[Client] Sent request to service '{}'", service_name);
}

void Client::receiveResponse(zmq::socket_t &socket, zmq::message_t &payloadMsg,
                             const std::string &service_name)
{
  zmq::message_t statusMsg;

  if (!socket.recv(statusMsg, zmq::recv_flags::none))
  {
    zlc::error("Timeout waiting for response from service {}", service_name);
    return;
  }

  if (!statusMsg.more())
  {
    zlc::error("No payload frame received for service response from {}", service_name);
    return;
  }

  if (!socket.recv(payloadMsg, zmq::recv_flags::none))
  {
    zlc::error("Timeout waiting for payload from service {}", service_name);
    return;
  }

  if (payloadMsg.more())
  {
    zlc::error("More frames received than expected from service {}", service_name);
  }
}

void Client::waitForService(const std::string &service_name, int max_wait_ms,
                            int check_interval_ms)
{
  auto &node = ZeroLanComNode::instance();
  int waited_ms = 0;

  while (waited_ms < max_wait_ms)
  {
    auto serviceInfoPtr = node.nodesManager.getServiceInfo(service_name);

    if (serviceInfoPtr != nullptr)
    {
      zlc::info("[Client] Service '{}' is now available.", service_name);
      return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms));

    waited_ms += check_interval_ms;
  }

  zlc::warn("[Client] Timeout waiting for service '{}'", service_name);
}

} // namespace zlc
