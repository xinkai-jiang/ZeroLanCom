#pragma once

// Core headers
#include "zerolancom_node.hpp"
#include "utils/logger.hpp"
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
void registerServiceHandler(
    const std::string& service_name,
    const std::function<std::string(const std::string&)>& handler)
{
    zlc::ZeroLanComNode::instance().registerServiceHandler<std::string, std::string>(service_name, handler);
}

template <typename HandlerT, typename ClassT>
void registerServiceHandler(
    const std::string& service_name,
    HandlerT handler,
    ClassT* instance)
{
    zlc::ZeroLanComNode::instance().registerServiceHandler<std::string, std::string>(service_name, std::bind(handler, instance));
}

template <typename HandlerT>
void registerSubscriberHandler(const std::string& name, HandlerT handler)
{
    zlc::ZeroLanComNode::instance().registerSubscriber<std::string>(name, handler);
}

template <typename HandlerT, typename ClassT>
void registerServiceHandler(
    const std::string& service_name,
    HandlerT handler,
    ClassT* instance)
{
    zlc::ZeroLanComNode::instance().registerServiceHandler<std::string, std::string>(service_name, std::bind(handler, instance));
}

}