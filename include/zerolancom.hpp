#pragma once

// Core headers
#include "utils/logger.hpp"
#include "sockets/client.hpp"
#include "sockets/publisher.hpp"

// dependent library headers
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <msgpack.hpp>

namespace zlc {

void init(const std::string& node_name, const std::string& ip_address)
{
    zlc::ZeroLanComNode::init(node_name, ip_address);
}

template <typename HandlerT>
void registerServiceHandler(const std::string& service_name, HandlerT handler)
{
    auto & node = zlc::ZeroLanComNode::instance();
    auto & localInfo = node.localInfo;
    auto & serviceManager = node.serviceManager;
    serviceManager.registerHandler(service_name, std::function(handler));
    localInfo.registerServices(service_name, serviceManager.service_port);
    zlc::info("Service {} registered at port {}", service_name, serviceManager.service_port);
}

template <typename HandlerT, typename ClassT>
void registerServiceHandler(
    const std::string& service_name,
    HandlerT handler,
    ClassT* instance)
{
    auto & node = zlc::ZeroLanComNode::instance();
    auto & localInfo = node.localInfo;
    auto & serviceManager = node.serviceManager;
    serviceManager.registerHandler(service_name, std::bind(handler, instance));
    localInfo.registerServices(service_name, serviceManager.service_port);
}

template <typename MessageType>
void registerSubscriberHandler(const std::string& name, void(*callback)(const MessageType&))
{
    auto& SubscriberManager = zlc::ZeroLanComNode::instance().subscriberManager;
    SubscriberManager.registerTopicSubscriber(name, std::function(callback));
}

// template <typename HandlerT, typename ClassT>
// void registerSubscriberHandler(
//     const std::string& service_name,
//     HandlerT handler,
//     ClassT* instance)
// {
//     auto& SubscriberManager = zlc::ZeroLanComNode::instance().subscriberManager;
//     SubscriberManager.registerTopicSubscriber(service_name,
//         std::bind(handler, instance, std::placeholders::_1));
// }

void sleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void spin() {
    auto& node = zlc::ZeroLanComNode::instance();
    try
    {
        while (node.isRunning()) {
            sleep(100);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    zlc::ZeroLanComNode::instance().stop();
}


    template<typename RequestType, typename ResponseType>
    static void request(
        const std::string& service_name,
        const RequestType& request,
        ResponseType& response){
        zlc::Client::request<RequestType, ResponseType>(service_name, request, response);
}
}