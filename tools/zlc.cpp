/**
 * @file zlc.cpp
 * @brief ZeroLanCom Node Discovery CLI Tool
 *
 * A passive CLI tool that listens for multicast heartbeats for 1 second,
 * then prints all discovered nodes and exits.
 *
 * Usage: zlc --ip <local_ip> [--node <group_name>] [--group <multicast_group>] [--port
 * <port>]
 */

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "zerolancom/nodes/multicast.hpp"
#include "zerolancom/nodes/node_info_manager.hpp"
#include "zerolancom/utils/zmq_utils.hpp"

void printUsage(const char *programName)
{
  std::cout
      << "Usage: " << programName << " --ip <local_ip> [options]\n"
      << "\n"
      << "Required:\n"
      << "  --ip <ip>         Local IP address to bind\n"
      << "\n"
      << "Options:\n"
      << "  -n, --node <name> Group name filter (default: zlc_default_group_name)\n"
      << "  -g, --group <ip>  Multicast group address (default: 224.0.0.1)\n"
      << "  -p, --port <port> Multicast port (default: 7720)\n"
      << "  -h, --help        Show this help message\n"
      << "\n"
      << "Example:\n"
      << "  " << programName << " --ip 192.168.1.100 --node my_group\n";
}

struct CLIOptions
{
  std::string localIP;
  std::string groupName = "zlc_default_group_name";
  std::string multicastGroup = "224.0.0.1";
  int port = 7720;
  bool showHelp = false;
};

bool parseArgs(int argc, char *argv[], CLIOptions &opts)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help")
    {
      opts.showHelp = true;
      return true;
    }
    else if (arg == "--ip" && i + 1 < argc)
    {
      opts.localIP = argv[++i];
    }
    else if ((arg == "-n" || arg == "--node") && i + 1 < argc)
    {
      opts.groupName = argv[++i];
    }
    else if ((arg == "-g" || arg == "--group") && i + 1 < argc)
    {
      opts.multicastGroup = argv[++i];
    }
    else if ((arg == "-p" || arg == "--port") && i + 1 < argc)
    {
      opts.port = std::stoi(argv[++i]);
    }
    else
    {
      std::cerr << "Unknown option or missing argument: " << arg << "\n";
      return false;
    }
  }

  if (!opts.showHelp && opts.localIP.empty())
  {
    std::cerr << "Error: --ip is required\n";
    return false;
  }

  return true;
}

int main(int argc, char *argv[])
{
  CLIOptions opts;

  if (!parseArgs(argc, argv, opts))
  {
    printUsage(argv[0]);
    return 1;
  }

  if (opts.showHelp)
  {
    printUsage(argv[0]);
    return 0;
  }

  std::cout << "Discovering nodes on group '" << opts.groupName << "'...\n";
  std::cout << "Multicast: " << opts.multicastGroup << ":" << opts.port << "\n";
  std::cout << "Local IP: " << opts.localIP << "\n\n";

  // Initialize the three required components for passive discovery
  zlc::ZMQContext::initExternal();
  zlc::NodeInfoManager::initExternal("zlc_discovery", opts.localIP);
  zlc::NodeInfoManager::instance().setGroupName(opts.groupName);
  zlc::MulticastReceiver::initExternal(opts.multicastGroup, opts.port, opts.localIP,
                                       opts.groupName);
  zlc::MulticastReceiver::instance().start();

  // Wait for 1 second to discover nodes
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Get all discovered nodes
  auto nodes = zlc::NodeInfoManager::instance().getAllNodes();

  if (nodes.empty())
  {
    std::cout << "No nodes discovered.\n";
  }
  else
  {
    std::cout << "Discovered " << nodes.size() << " node(s):\n";
    std::cout << "========================================\n\n";
    for (const auto &node : nodes)
    {
      node.printNodeInfo();
      std::cout << "\n";
    }
  }

  // Cleanup in reverse order
  zlc::MulticastReceiver::instance().stop();
  zlc::MulticastReceiver::destroy();
  zlc::NodeInfoManager::destroy();
  zlc::ZMQContext::destroy();

  return 0;
}
