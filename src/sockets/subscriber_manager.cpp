#include "zerolancom/sockets/subscriber_manager.hpp"

namespace zlc
{

SubscriberManager::SubscriberManager(NodeInfoManager &node_info_mgr)
    : node_info_mgr_(node_info_mgr)
{
  // Register callback to receive node/topic updates
  node_info_mgr_.registerUpdateCallback(std::bind(
      &SubscriberManager::updateTopicSubscriber, this, std::placeholders::_1));
}

SubscriberManager::~SubscriberManager()
{
  stop();
}

void SubscriberManager::start()
{
  is_running_ = true;
  poll_thread_ = std::thread(&SubscriberManager::pollLoop, this);
}

void SubscriberManager::stop()
{
  is_running_ = false;
  if (poll_thread_.joinable())
  {
    poll_thread_.join();
  }
}

std::vector<std::string> SubscriberManager::findTopicURLs(const std::string &topicName)
{
  std::vector<std::string> urls;

  auto infos = node_info_mgr_.getPublisherInfo(topicName);

  for (const auto &t : infos)
  {
    urls.push_back(fmt::format("tcp://{}:{}", t.ip, t.port));
  }

  return urls;
}

void SubscriberManager::updateTopicSubscriber(const NodeInfo &nodeInfo)
{
  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto &topic : nodeInfo.topics)
  {
    for (auto &sub : subscribers_)
    {
      if (sub.topicName != topic.name)
        continue;

      std::string url = fmt::format("tcp://{}:{}", topic.ip, topic.port);

      if (std::find(sub.publisherURLs.begin(), sub.publisherURLs.end(), url) !=
          sub.publisherURLs.end())
      {
        continue; // already connected
      }

      sub.socket->connect(url);
      sub.publisherURLs.push_back(url);

      zlc::info("[SubscriberManager] '{}' connected to {}", topic.name, url);
    }
  }
}

void SubscriberManager::pollLoop()
{
  zlc::info("[SubscriberManager] Poll thread started.");

  while (is_running_)
  {
    std::vector<zmq::pollitem_t> poll_items;
    std::vector<Subscriber *> subs;

    {
      std::lock_guard<std::mutex> lock(mutex_);
      poll_items.reserve(subscribers_.size());
      subs.reserve(subscribers_.size());

      for (auto &sub : subscribers_)
      {
        poll_items.push_back({static_cast<void *>(*sub.socket), 0, ZMQ_POLLIN, 0});
        subs.push_back(&sub);
      }
    }

    zmq::poll(poll_items.data(), poll_items.size(), std::chrono::milliseconds(100));

    for (size_t i = 0; i < poll_items.size(); ++i)
    {
      if (poll_items[i].revents & ZMQ_POLLIN)
      {
        zmq::message_t msg;
        if (!subs[i]->socket->recv(msg, zmq::recv_flags::none))
        {
          continue;
        }

        ByteView view{static_cast<const uint8_t *>(msg.data()), msg.size()};

        subs[i]->callback(view);
      }
    }
  }
}

} // namespace zlc
