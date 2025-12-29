#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <zmq.hpp>

#include "zerolancom/serialization/serializer.hpp"
#include "zerolancom/utils/logger.hpp"
#include "zerolancom/utils/request_result.hpp"
#include "zerolancom/utils/zmq_utils.hpp"

namespace zlc
{

/**
 * @brief ServiceManager handles incoming RPC service requests.
 *
 * Design notes:
 * - Owns a REP socket and a worker thread.
 * - Template registerHandler functions must remain header-only.
 * - Non-template functions are implemented in service_manager.cpp.
 */
class ServiceManager
{
public:
  int service_port{0};

  explicit ServiceManager(const std::string &ip);
  ~ServiceManager();

  void start();
  void stop();

  /**
   * @brief Register a request-response handler.
   */
  template <typename RequestType, typename ResponseType>
  void registerHandler(const std::string &name,
                       const std::function<ResponseType(const RequestType &)> &func)
  {
    handlers_[name] = [func](const ByteView &payload) -> std::vector<uint8_t>
    {
      RequestType req;
      decode(payload, req);

      ResponseType resp = func(req);

      ByteBuffer out;
      encode(resp, out);
      return std::vector<uint8_t>(out.data, out.data + out.size);
    };
  }

  /**
   * @brief Register a fire-and-forget handler with request payload.
   */
  template <typename RequestType>
  void registerHandler(const std::string &name,
                       const std::function<void(const RequestType &)> &func)
  {
    handlers_[name] = [func](const ByteView &payload) -> std::vector<uint8_t>
    {
      RequestType req;
      decode(payload, req);
      func(req);
      return {};
    };
  }

  /**
   * @brief Register a handler that returns a response without request payload.
   */
  template <typename ResponseType>
  void registerHandler(const std::string &name,
                       const std::function<ResponseType()> &func)
  {
    handlers_[name] = [func](const ByteView &) -> std::vector<uint8_t>
    {
      ResponseType ret = func();
      ByteBuffer out;
      encode(ret, out);
      return std::vector<uint8_t>(out.data, out.data + out.size);
    };
  }

  /**
   * @brief Register a fire-and-forget handler without payload.
   */
  void registerHandler(const std::string &name, const std::function<void()> &func);

  void handleRequest(const std::string &service_name, const ByteView &payload,
                     Response &response);

  void clearHandlers();
  void removeHandler(const std::string &name);

  // Non-copyable, movable
  ServiceManager(const ServiceManager &) = delete;
  ServiceManager &operator=(const ServiceManager &) = delete;
  ServiceManager(ServiceManager &&) = default;
  ServiceManager &operator=(ServiceManager &&) = default;

private:
  // Thread entry for handling service requests
  void responseSocketThread();

private:
  std::unordered_map<std::string, std::function<std::vector<uint8_t>(const ByteView &)>>
      handlers_;

  bool is_running{false};

  zmq::socket_t res_socket_;
  static constexpr int SOCKET_TIMEOUT_MS = 100;

  std::thread service_thread_;
};

} // namespace zlc
