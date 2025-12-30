#pragma once

#include <functional>
#include <string>

// Core headers
// #include "zerolancom/nodes/zerolancom_node.hpp"
// #include "zerolancom/nodes/node_info.hpp"
#include "zerolancom/sockets/client.hpp"
#include "zerolancom/sockets/publisher.hpp"
#include "zerolancom/utils/logger.hpp"

namespace zlc
{

// =======================
// Non-template API (cpp)
// =======================

void init(const std::string &node_name, const std::string &ip_address);
void sleep(int ms);
void spin();

// =======================
// Template APIs (header-only)
// =======================

template <typename HandlerT>
void registerServiceHandler(const std::string &service_name, HandlerT handler)
{
  auto &node = zlc::ZeroLanComNode::instance();
  auto &localInfo = node.localInfo;
  auto &serviceManager = node.serviceManager;

  serviceManager.registerHandler(service_name, std::function(handler));
  localInfo.registerServices(service_name, serviceManager.service_port);

  zlc::info("Service {} registered at port {}", service_name,
            serviceManager.service_port);
}

template <typename HandlerT, typename ClassT>
void registerServiceHandler(const std::string &service_name, HandlerT handler,
                            ClassT *instance)
{
  auto &node = zlc::ZeroLanComNode::instance();
  auto &localInfo = node.localInfo;
  auto &serviceManager = node.serviceManager;

  serviceManager.registerHandler(service_name, handler, instance);

  localInfo.registerServices(service_name, serviceManager.service_port);
}

/**
 * @brief Block until the service becomes available or timeout expires.
 */
void waitForService(const std::string &service_name, int max_wait_ms = 1000,
                    int check_interval_ms = 10)
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

template <typename MessageType>
void registerSubscriberHandler(const std::string &name,
                               void (*callback)(const MessageType &))
{
  auto &subscriberManager = zlc::ZeroLanComNode::instance().subscriberManager;

  subscriberManager.registerTopicSubscriber(name, std::function(callback));
}

// template <typename MessageType>
// void registerSubscriberHandler(const std::string &name, std::function<void(const
// MessageType &)> callback)
// {
//   auto &subscriberManager = zlc::ZeroLanComNode::instance().subscriberManager;

//   subscriberManager.registerTopicSubscriber(name, callback);
// }

template <typename RequestType, typename ResponseType>
void request(const std::string &service_name, const RequestType &request,
             ResponseType &response)
{
  waitForService(service_name);
  Client::request<RequestType, ResponseType>(service_name, request, response);
}

} // namespace zlc
