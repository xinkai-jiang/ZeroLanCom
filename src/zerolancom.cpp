#include "zerolancom/zerolancom.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace zlc
{

void init(const std::string &node_name, const std::string &ip_address)
{
  Logger::init(false);
  Logger::setLevel(LogLevel::INFO);

  ZeroLanComNode::init(node_name, ip_address, "224.0.0.1", 7720);
}

void sleep(int ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void spin()
{
  auto &node = zlc::ZeroLanComNode::instance();

  try
  {
    while (node.isRunning())
    {
      sleep(100);
    }
  }
  catch (const std::exception &e)
  {
    zlc::error("Exception caught in spin: {}", e.what());
  }

  node.stop();
}

} // namespace zlc
