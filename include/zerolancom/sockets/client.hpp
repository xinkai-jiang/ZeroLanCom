#pragma once

#include <string>

#include <zmq.hpp>

#include "zerolancom/serialization/serializer.hpp"
#include "zerolancom/utils/logger.hpp"
#include "zerolancom/utils/zmq_utils.hpp"

namespace zlc
{

/**
 * @brief Client is a stateless service proxy used to perform RPC calls.
 *
 * Design notes:
 * - Non-template functions are declared here and defined in client.cpp.
 * - Template functions must remain header-only.
 * - This class relies on ZMQContext singleton being initialized.
 */
class Client
{
public:
  // Connect a ZMQ socket to the given service
  static void connect(ZMQSocket &socket, const std::string &service_name);

  // Send a multipart request (service name + payload)
  static void sendRequest(const std::string &service_name, const ByteView &payload,
                          ZMQSocket &socket);
  // Receive multipart response and extract payload
  static void receiveResponse(ZMQSocket &socket, zmq::message_t &payloadMsg,
                              const std::string &service_name);

  /**
   * @brief Perform a blocking service request.
   *
   * Requirements:
   * - RequestType and ResponseType must be serializable via encode/decode.
   * - This function blocks until a response is received or an error occurs.
   */
  template <typename RequestType, typename ResponseType>
  static void request(const std::string &service_name, const RequestType &request,
                      ResponseType &response)
  {
    // Create a REQ socket for this request
    ZMQSocket req_socket = ZMQContext::createTempSocket(zmq::socket_type::req);

    // Resolve service and connect
    connect(req_socket, service_name);

    // Serialize request
    ByteBuffer out;
    encode(request, out);

    // Send request frames
    sendRequest(service_name, ByteView{out.data, out.size}, req_socket);

    // Receive response payload
    zmq::message_t payloadMsg;
    receiveResponse(req_socket, payloadMsg, service_name);

    // Deserialize response
    ByteView payload{static_cast<const uint8_t *>(payloadMsg.data()),
                     payloadMsg.size()};
    if (payload.size != 0)
    {
      decode(payload, response);
    }
    req_socket.close();
    zlc::info("[Client] Received response from service '{}'", service_name);
  }
};

} // namespace zlc
