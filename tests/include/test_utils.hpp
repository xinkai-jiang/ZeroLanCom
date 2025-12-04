#pragma once
#include "lancom_node.hpp"
#include "utils/logger.hpp"


void topicCallback(const std::string& msg) {
    LOG_INFO("Received message on subscribed topic: {}", msg);
}

std::string serviceHandler(const std::string& request) {
    LOG_INFO("Service request received.");
    return request; // Echo the received request
}