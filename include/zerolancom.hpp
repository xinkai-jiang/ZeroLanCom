#pragma once

#include <functional>
#include <string>

// Core headers
#include "nodes/node_info.hpp"
#include "nodes/zerolancom_node.hpp"
#include "sockets/client.hpp"
#include "sockets/publisher.hpp"
#include "utils/logger.hpp"
#include "utils/request_result.hpp"

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

  serviceManager.registerHandler(service_name, std::bind(handler, instance));

  localInfo.registerServices(service_name, serviceManager.service_port);
}

template <typename MessageType>
void registerSubscriberHandler(const std::string &name,
                               void (*callback)(const MessageType &))
{
  auto &subscriberManager = zlc::ZeroLanComNode::instance().subscriberManager;

  subscriberManager.registerTopicSubscriber(name, std::function(callback));
}

template <typename RequestType, typename ResponseType>
void request(const std::string &service_name, const RequestType &request,
             ResponseType &response)
{
  zlc::Client::request<RequestType, ResponseType>(service_name, request, response);
}

} // namespace zlc
