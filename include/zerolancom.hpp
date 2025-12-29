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
    zlc::ZeroLanComNode::instance().registerServiceHandler(service_name, handler);
}

template <typename HandlerT, typename ClassT>
void registerServiceHandler(
    const std::string& service_name,
    HandlerT handler,
    ClassT* instance)
{
    zlc::ZeroLanComNode::instance().registerServiceHandler(service_name, handler, instance);
}

template <typename HandlerT>
void registerSubscriberHandler(const std::string& name, HandlerT handler)
{
    zlc::ZeroLanComNode::instance().registerSubscriber(name, handler);
}

template <typename HandlerT, typename ClassT>
void registerSubscriberHandler(
    const std::string& service_name,
    HandlerT handler,
    ClassT* instance)
{
    zlc::ZeroLanComNode::instance().registerSubscriber(service_name, handler, instance);
}

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