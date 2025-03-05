#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "lancom/sockets/publisher.hpp"
#include "lancom/types.hpp"

namespace lancom {

// Data generator for streaming
template<typename T>
using UpdateFunction = std::function<T()>;

// Encoder for stream messages
template<typename T>
using MessageEncoder = std::function<BytesMessage(const T&)>;

// Streamer implementation for continuous data publishing
template<typename T>
class Streamer : public Publisher {
public:
    // Create a new streamer
    Streamer(
        const std::string& topic_name,
        UpdateFunction<T> update_func,
        int fps,
        MessageEncoder<T> msg_encoder,
        bool start_streaming = false);
    
    // Destructor
    ~Streamer() override = default;
    
    // Start streaming data
    void start_streaming();
    
    // Stop streaming
    void stop_streaming();

protected:
    // Generate a byte message from update function
    BytesMessage generate_byte_msg();
    
    // Update loop for publishing data
    Task<void> update_loop();

private:
    // Update parameters
    std::chrono::microseconds dt_;
    UpdateFunction<T> update_func_;
    MessageEncoder<T> msg_encoder_;
    std::string topic_byte_;
};

// Template implementation
template<typename T>
Streamer<T>::Streamer(
    const std::string& topic_name,
    UpdateFunction<T> update_func,
    int fps,
    MessageEncoder<T> msg_encoder,
    bool start_streaming)
    : Publisher(topic_name),
      dt_(std::chrono::microseconds(1000000 / fps)),
      update_func_(std::move(update_func)),
      msg_encoder_(std::move(msg_encoder)),
      topic_byte_(name_) {
    
    if (start_streaming) {
        this->start_streaming();
    }
}

template<typename T>
void Streamer<T>::start_streaming() {
    node_->submit_task([this]() {
        return update_loop();
    });
}

template<typename T>
void Streamer<T>::stop_streaming() {
    running_ = false;
}

template<typename T>
BytesMessage Streamer<T>::generate_byte_msg() {
    return msg_encoder_(update_func_());
}

template<typename T>
Task<void> Streamer<T>::update_loop() {
    running_ = true;
    auto last = std::chrono::steady_clock::now();
    
    info("Topic " + name_ + " starts streaming");
    
    while (running_) {
        try {
            auto now = std::chrono::steady_clock::now();
            auto diff = now - last;
            
            if (diff < dt_) {
                auto sleep_time = dt_ - diff;
                co_await std::suspend_always{};
                std::this_thread::sleep_for(sleep_time);
            }
            
            last = std::chrono::steady_clock::now();
            
            // Generate and send message
            co_await send_bytes_async(generate_byte_msg());
        }
        catch (const std::exception& e) {
            error("Error when streaming " + name_ + ": " + e.what());
        }
    }
    
    info("Streamer for topic " + name_ + " is stopped");
}

} // namespace lancom
