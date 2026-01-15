#include "zerolancom/sockets/service_manager.hpp"
#include "zerolancom/utils/exception.hpp"

namespace zlc
{

ServiceManager::ServiceManager(zmq::context_t &zmq_context, const std::string &ip)
    : router_socket_(zmq_context, zmq::socket_type::rep)
{
  router_socket_.set(zmq::sockopt::rcvtimeo, SOCKET_TIMEOUT_MS);
  router_socket_.bind("tcp://" + ip + ":0");
  service_port = getBoundPort(router_socket_);

  zlc::info("[ServiceManager] ServiceManager bound to port {}", service_port);
}

ServiceManager::~ServiceManager()
{
  stop();
  router_socket_.close();
}

void ServiceManager::start(ThreadPool &pool)
{
  poll_task_ = std::make_unique<PeriodicTask>([this]() { this->pollOnce(); }, 100,
                                              pool); // Poll every 100ms

  poll_task_->start();
}

void ServiceManager::stop()
{
  if (poll_task_)
  {
    poll_task_->stop();
  }
}

void ServiceManager::handleRequest(const std::string &service_name,
                                   const ByteView &payload, Response &response)
{
  auto it = handlers_.find(service_name);

  zlc::info("[ServiceManager] Handling request for service '{}'", service_name);

  if (it == handlers_.end())
  {
    response.code = ResponseStatus::NOSERVICE;
    return;
  }

  response.code = ResponseStatus::SUCCESS;

  try
  {
    response.payload = it->second(payload);
  }
  catch (const DecodeException &e)
  {
    zlc::error("[ServiceManager] ZMQ error while handling service '{}': {}",
               service_name, e.what());
    response.code = ResponseStatus::INVALID_RESPONSE;
  }
  catch (const EncodeException &e)
  {
    zlc::error("[ServiceManager] ZMQ error while handling service '{}': {}",
               service_name, e.what());
    response.code = ResponseStatus::INVALID_REQUEST;
  }
  catch (const std::exception &e)
  {
    zlc::error("[ServiceManager] Exception while handling service '{}': {}",
               service_name, e.what());
    response.code = ResponseStatus::SERVICE_FAIL;
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

void ServiceManager::processRequest(const std::string &identity,
                                    const std::string &service_name,
                                    const ByteView &payload)
{
  Response response;
  handleRequest(service_name, payload, response);

  // Send response back via REP socket
  router_socket_.send(zmq::buffer(response.code), zmq::send_flags::sndmore);
  router_socket_.send(zmq::buffer(response.payload), zmq::send_flags::none);
}

void ServiceManager::pollOnce()
{
  try
  {
    zmq::message_t service_name_msg;

    if (!router_socket_.recv(service_name_msg, zmq::recv_flags::none))
    {
      return;
    }

    std::string service_name = decodeServiceHeader(
        ByteView{static_cast<const uint8_t *>(service_name_msg.data()),
                 service_name_msg.size()});

    if (!service_name_msg.more())
    {
      zlc::warn("[ServiceManager] Missing payload frame");
      return;
    }

    zmq::message_t payload_msg;
    if (!router_socket_.recv(payload_msg, zmq::recv_flags::none))
    {
      return;
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
    router_socket_.send(zmq::buffer(response.code), zmq::send_flags::sndmore);
    router_socket_.send(zmq::buffer(response.payload), zmq::send_flags::none);
  }
  catch (const zmq::error_t &e)
  {
    if (e.num() == ETERM)
    {
      zlc::info("[ServiceManager] Context terminated during poll");
      return;
    }
    zlc::error("[ServiceManager] ZMQ error: {}", e.what());
  }
}

} // namespace zlc
