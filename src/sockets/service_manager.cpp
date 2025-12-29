#include "sockets/service_manager.hpp"

namespace zlc
{

ServiceManager::ServiceManager(const std::string &ip)
    : res_socket_(ZmqContext::instance(), zmq::socket_type::rep)
{
  res_socket_.set(zmq::sockopt::rcvtimeo, SOCKET_TIMEOUT_MS);
  res_socket_.bind("tcp://" + ip + ":0");

  service_port = getBoundPort(res_socket_);

  zlc::info("[ServiceManager] ServiceManager bound to port {}", service_port);
}

ServiceManager::~ServiceManager()
{
  stop();
  res_socket_.close();
}

void ServiceManager::start()
{
  is_running = true;
  service_thread_ = std::thread(&ServiceManager::responseSocketThread, this);
}

void ServiceManager::stop()
{
  is_running = false;
  if (service_thread_.joinable())
  {
    service_thread_.join();
  }
}

void ServiceManager::registerHandler(const std::string &name,
                                     const std::function<void()> &func)
{
  handlers_[name] = [func](const ByteView &) -> std::vector<uint8_t>
  {
    func();
    return {};
  };
}

void ServiceManager::handleRequest(const std::string &service_name,
                                   const ByteView &payload, Response &response)
{
  auto it = handlers_.find(service_name);

  zlc::info("[ServiceManager] Handling request for service '{}'", service_name);

  if (it == handlers_.end())
  {
    response.code = ResponseStatus::FAIL;
    return;
  }

  response.code = ResponseStatus::SUCCESS;

  try
  {
    response.payload = it->second(payload);
  }
  catch (const std::exception &e)
  {
    zlc::error("[ServiceManager] Exception while handling service '{}': {}",
               service_name, e.what());
    response.code = ResponseStatus::FAIL;
  }
}

void ServiceManager::clearHandlers()
{
  handlers_.clear();
}

void ServiceManager::removeHandler(const std::string &name)
{
  handlers_.erase(name);
}

void ServiceManager::responseSocketThread()
{
  while (is_running)
  {
    zmq::message_t service_name_msg;

    if (!res_socket_.recv(service_name_msg, zmq::recv_flags::none))
    {
      continue;
    }

    std::string service_name = decodeServiceHeader(
        ByteView{static_cast<const uint8_t *>(service_name_msg.data()),
                 service_name_msg.size()});

    if (!service_name_msg.more())
    {
      zlc::warn("[ServiceManager] Missing payload frame");
      continue;
    }

    zmq::message_t payload_msg;
    if (!res_socket_.recv(payload_msg, zmq::recv_flags::none))
    {
      continue;
    }

    ByteView payload{static_cast<const uint8_t *>(payload_msg.data()),
                     payload_msg.size()};

    if (payload_msg.more())
    {
      zlc::warn("[ServiceManager] Extra frames received");
    }

    Response response;
    handleRequest(service_name, payload, response);

    // Send response frames
    res_socket_.send(zmq::buffer(service_name), zmq::send_flags::sndmore);
    res_socket_.send(zmq::buffer(response.payload), zmq::send_flags::none);
  }
}

} // namespace zlc
