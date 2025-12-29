#include "zerolancom/nodes/multicast.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <unistd.h>

#include "zerolancom/utils/logger.hpp"

namespace zlc
{

/* ================= MulticastSender ================= */

MulticastSender::MulticastSender(const std::string &group, int port,
                                 const std::string &localIP)
{
  sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  int ttl = 1;
  setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

  in_addr local{};
  local.s_addr = inet_addr(localIP.c_str());
  setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_IF, &local, sizeof(local));

  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr = inet_addr(group.c_str());
}

MulticastSender::~MulticastSender()
{
  stop();
  if (sock_ >= 0)
  {
    close(sock_);
  }
}

void MulticastSender::start(const LocalNodeInfo &localInfo)
{
  running_ = true;
  multicastSendThread_ = std::thread(
      [this, &localInfo]()
      {
        while (running_)
        {
          auto msg = localInfo.createHeartbeat();
          sendHeartbeat(msg);
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
      });
}

void MulticastSender::stop()
{
  running_ = false;
  if (multicastSendThread_.joinable())
  {
    multicastSendThread_.join();
  }
}

void MulticastSender::sendHeartbeat(const std::vector<uint8_t> &msg)
{
  sendto(sock_, msg.data(), msg.size(), 0, reinterpret_cast<sockaddr *>(&addr_),
         sizeof(addr_));
}

/* ================= MulticastReceiver ================= */

MulticastReceiver::MulticastReceiver(const std::string &group, int port,
                                     const std::string &localIP)
{
  sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  int reuse = 1;
  setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  bind(sock_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

  ip_mreq mreq{};
  mreq.imr_multiaddr.s_addr = inet_addr(group.c_str());
  mreq.imr_interface.s_addr = inet_addr(localIP.c_str());
  setsockopt(sock_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}

MulticastReceiver::~MulticastReceiver()
{
  stop();
  if (sock_ >= 0)
  {
    close(sock_);
  }
}

void MulticastReceiver::start(NodeInfoManager &nodeManager)
{
  running_ = true;
  multicastReceiveThread_ = std::thread(
      [this, &nodeManager]()
      {
        std::vector<uint8_t> buf(256);
        sockaddr_in src{};
        socklen_t slen = sizeof(src);

        while (running_)
        {
          int n = recvfrom(sock_, buf.data(), static_cast<int>(buf.size()), 0,
                           reinterpret_cast<sockaddr *>(&src), &slen);

          if (n <= 0)
            continue;

          std::string ip = inet_ntoa(src.sin_addr);
          NodeInfo info =
              NodeInfo::decode(ByteView{buf.data(), static_cast<size_t>(n)});
          info.ip = ip;

          nodeManager.processHeartbeat(info);
          nodeManager.checkHeartbeats();

          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        zlc::info("Multicast receiver stopped.");
      });
}

void MulticastReceiver::stop()
{
  running_ = false;
  if (multicastReceiveThread_.joinable())
  {
    multicastReceiveThread_.join();
  }
}

} // namespace zlc
