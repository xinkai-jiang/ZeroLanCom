#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>
#include <zmq.hpp>

#include "zerolancom/nodes/node_info.hpp"
#include "zerolancom/nodes/node_info_manager.hpp"
#include "zerolancom/serialization/serializer.hpp"
#include "zerolancom/utils/logger.hpp"
#include "zerolancom/utils/zmq_utils.hpp"

namespace zlc
{

/**
 * @brief SubscriberManager manages topic subscriptions and message dispatch.
 *
 * Design notes:
 * - Automatically discovers publishers via NodeInfoManager callbacks.
 * - Uses one SUB socket per topic.
 * - Template subscription API must remain header-only.
 */
class SubscriberManager
{
public:
  explicit SubscriberManager(NodeInfoManager &node_info_mgr);
  ~SubscriberManager();

  /**
   * @brief Register a subscriber callback for a topic.
   *
   * Requirements:
   * - MessageType must be decodable via decode().
   * - Callback is executed in the polling thread.
   */
  template <typename MessageType>
  void registerTopicSubscriber(const std::string &topicName,
                               void (*callback)(const MessageType &))
  {
    std::lock_guard<std::mutex> lock(mutex_);

    Subscriber sub;
    sub.topicName = topicName;

    // Wrap typed callback into raw ByteView callback
    sub.callback = [callback](const ByteView &view)
    {
      MessageType msg;
      decode(view, msg);
      callback(msg);
    };

    sub.socket =
        std::make_unique<zmq::socket_t>(ZmqContext::instance(), zmq::socket_type::sub);
    sub.socket->set(zmq::sockopt::subscribe, "");
    auto urls = findTopicURLs(topicName);
    for (const auto &url : urls)
    {
      sub.socket->connect(url);
      zlc::info("[SubscriberManager] '{}' connected to {}", topicName, url);
      sub.publisherURLs.push_back(url);
    }
    subscribers_.push_back(std::move(sub));
  }

  template <typename MessageType, typename ClassT>
  void registerTopicSubscriber(const std::string &topicName,
                               void (ClassT::*callback)(const MessageType &),
                               ClassT *instance)
  {
    std::lock_guard<std::mutex> lock(mutex_);

    Subscriber sub;
    sub.topicName = topicName;

    // Wrap typed callback into raw ByteView callback
    sub.callback = [callback, instance](const ByteView &view)
    {
      MessageType msg;
      decode(view, msg);
      (instance->*callback)(msg);
    };

    sub.socket =
        std::make_unique<zmq::socket_t>(ZmqContext::instance(), zmq::socket_type::sub);
    sub.socket->set(zmq::sockopt::subscribe, "");
    auto urls = findTopicURLs(topicName);
    for (const auto &url : urls)
    {
      sub.socket->connect(url);
      zlc::info("[SubscriberManager] '{}' connected to {}", topicName, url);
      sub.publisherURLs.push_back(url);
    }
    subscribers_.push_back(std::move(sub));
  }

  // Start polling thread
  void start();

  // Stop polling thread
  void stop();

  // Called by NodeInfoManager when a node announces new topics
  void updateTopicSubscriber(const NodeInfo &nodeInfo);

private:
  // Find all publisher endpoints for a topic
  std::vector<std::string> findTopicURLs(const std::string &topicName);

  // Poll loop running in a dedicated thread
  void pollLoop();

private:
  struct Subscriber
  {
    std::string topicName;
    std::vector<std::string> publisherURLs;
    std::function<void(const ByteView &)> callback;
    std::unique_ptr<zmq::socket_t> socket;
  };

private:
  NodeInfoManager &node_info_mgr_;

  std::vector<Subscriber> subscribers_;
  std::mutex mutex_;

  std::thread poll_thread_;
  bool is_running_{false};
};

} // namespace zlc
