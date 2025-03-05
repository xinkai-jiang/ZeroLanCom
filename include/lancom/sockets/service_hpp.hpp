#pragma once

#include <functional>
#include <memory>
#include <string>

#include "lancom/sockets/publisher.hpp"
#include "lancom/types.hpp"

namespace lancom {

// Request/response encoders and decoders
template<typename T>
using RequestDecoder = std::function<T(const BytesMessage&)>;

template<typename T>
using ResponseEncoder = std::function<BytesMessage(const T&)>;

template<typename Req, typename Resp>
using ServiceHandler = std::function<Resp(const Req&)>;

// Service implementation
class Service : public AbstractLanComSocket {
public:
    // Create a new service
    template<typename Req, typename Resp>
    Service(
        const std::string& service_name,
        RequestDecoder<Req> request_decoder,
        ResponseEncoder<Resp> response_encoder,
        ServiceHandler<Req, Resp> callback);
    
    // Destructor
    ~Service() override = default;

protected:
    // Implement shutdown
    void on_shutdown() override;
    
    // Callback wrapper for handling requests
    template<typename Req, typename Resp>
    BytesMessage callback_wrapper(
        const BytesMessage& msg,
        RequestDecoder<Req> request_decoder,
        ResponseEncoder<Resp> response_encoder,
        ServiceHandler<Req, Resp> handler);

private:
    // Service callback for node registration
    std::function<BytesMessage(const BytesMessage&)> service_callback_;
};

// Template implementation
template<typename Req, typename Resp>
Service::Service(
    const std::string& service_name,
    RequestDecoder<Req> request_decoder,
    ResponseEncoder<Resp> response_encoder,
    ServiceHandler<Req, Resp> callback)
    : AbstractLanComSocket(service_name, ComponentTypeEnum::SERVICE, false) {
    
    // Set up the socket reference (shared with node)
    set_up_socket(node_->get_service_socket());
    
    // Check if service is already registered
    for (const auto& service_info : node_->get_node_info().services) {
        if (service_info.name == name_) {
            throw std::runtime_error("Service has been registered locally");
        }
    }
    
    if (node_->nodes_map_->get_service_info(service_name).socketID != "") {
        throw std::runtime_error("Service has been registered");
    }
    
    // Create the service callback
    service_callback_ = [this, request_decoder, response_encoder, callback](const BytesMessage& msg) {
        return callback_wrapper(msg, request_decoder, response_encoder, callback);
    };
    
    // Register with node
    node_->register_service(name_, service_callback_);
    
    info("\"" + name_ + "\" Service is started");
}

template<typename Req, typename Resp>
BytesMessage Service::callback_wrapper(
    const BytesMessage& msg,
    RequestDecoder<Req> request_decoder,
    ResponseEncoder<Resp> response_encoder,
    ServiceHandler<Req, Resp> handler) {
    
    try {
        // Decode request
        Req request = request_decoder(msg);
        
        // Process request
        Resp result = handler(request);
        
        // Encode response
        return response_encoder(result);
    }
    catch (const std::exception& e) {
        error("Error processing service request: " + std::string(e.what()));
        return BytesMessage(lancom_msg_to_string(LanComMsg::ERROR).begin(),
                           lancom_msg_to_string(LanComMsg::ERROR).end());
    }
}

} // namespace lancom
