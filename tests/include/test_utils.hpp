#pragma once
#include "lancom_node.hpp"
#include "zerolancom/utils/logger.hpp"

void topicCallback(const std::string &msg)
{
  zlc::info("Received message on subscribed topic: {}", msg);
}

std::string serviceHandler(const std::string &request)
{
  zlc::info("Service request received.");
  return request; // Echo the received request
}