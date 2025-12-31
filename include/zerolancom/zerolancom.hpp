#pragma once

#include <functional>
#include <string>

// Core headers
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

/**
 * @brief Block until the service becomes available or timeout expires.
 */
void waitForService(const std::string &service_name, int max_wait_ms = 1000,
                    int check_interval_ms = 10);

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

  node.serviceManager.registerHandler(service_name, handler, instance);

  uint16_t port = node.serviceManager.service_port;
  node.localInfo.registerServices(service_name, port);
}

template <typename HandlerT>
void registerSubscriberHandler(const std::string &name, HandlerT callback)
{
  auto &subscriberManager = ZeroLanComNode::instance().subscriberManager;
  subscriberManager.registerTopicSubscriber(name, callback);
}

template <typename HandlerT, typename ClassT>
void registerSubscriberHandler(const std::string &name, HandlerT callback,
                               ClassT *instance)
{
  auto &subscriberManager = ZeroLanComNode::instance().subscriberManager;
  subscriberManager.registerTopicSubscriber(name, callback, instance);
}

template <typename RequestType, typename ResponseType>
void request(const std::string &service_name, const RequestType &req, ResponseType &res)
{
  waitForService(service_name);
  Client::request<RequestType, ResponseType>(service_name, req, res);
}

template <typename RequestType>
void request(const std::string &service_name, const RequestType &req, Empty &)
{
  Empty zlc_empty;
  request<RequestType, Empty>(service_name, req, zlc_empty);
}

} // namespace zlc
