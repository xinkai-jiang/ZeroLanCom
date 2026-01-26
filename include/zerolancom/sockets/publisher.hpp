#pragma once

#include <memory>
#include <string>

#include <zmq.hpp>

#include "zerolancom/nodes/node_info_manager.hpp"
#include "zerolancom/serialization/serializer.hpp"
#include "zerolancom/utils/logger.hpp"
#include "zerolancom/utils/zmq_utils.hpp"

namespace zlc
{

/**
 * @brief Publisher is a templated PUB socket wrapper.
 *
 * Design notes:
 * - This is a template class and MUST remain header-only.
 * - All methods are defined inline to allow template instantiation.
 * - Each Publisher owns its own ZMQ PUB socket.
 */
template <typename T> class Publisher
{
public:
  /**
   * @brief Construct a publisher for a given topic.
   *
   * @param topic_name Logical topic name
   * @param with_local_namespace If true, prefix with "lc.local."
   *
   * Behavior:
   * - Binds to tcp://<local_ip>:0 (ephemeral port)
   * - Registers the topic with ZeroLanComNode
   */
  explicit Publisher(const std::string &topic_name, bool with_local_namespace = false)
  {
    const std::string full_topic_name =
        with_local_namespace ? "lc.local." + topic_name : topic_name;

    // Create PUB socket
    socket_ = ZMQContext::createSocket(zmq::socket_type::pub);

    // Bind to an ephemeral port
    const std::string address = NodeInfoManager::instance().getLocalNodeInfo().ip;

    socket_->bind("tcp://" + address + ":0");

    // Query bound port
    port_ = getBoundPort(*socket_);

    zlc::info("[Publisher] Publisher for topic '{}' bound to port {}", full_topic_name,
              port_);

    // Register topic in node discovery
    NodeInfoManager::instance().registerLocalTopic(full_topic_name,
                                                   static_cast<uint16_t>(port_));
  }

  // Destructor
  ~Publisher() = default;

  /**
   * @brief Publish a message to the topic.
   *
   * Requirements:
   * - T must be serializable via encode()
   * - This call is non-blocking (ZMQ PUB semantics)
   */
  void publish(const T &msg)
  {
    ByteBuffer out;
    encode(msg, out);

    socket_->send(zmq::buffer(out.data, out.size), zmq::send_flags::none);
  }

private:
  // Owned PUB socket
  ZMQSocket *socket_;

  // Bound port number
  int port_{0};
};

} // namespace zlc
