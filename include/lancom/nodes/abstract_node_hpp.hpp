#pragma once

#include <atomic>
#include <coroutine>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <zmq.hpp>
#include <zmq_addon.hpp>

#include "lancom/types.hpp"

namespace lancom {

// Forward declarations
class NodesMap;

// Coroutine task type for asynchronous operations
template <typename T>
class Task {
public:
    struct promise_type {
        T result;
        std::exception_ptr exception;

        Task get_return_object() { 
            return Task(std::coroutine_handle<promise_type>::from_promise(*this)); 
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T value) { result = std::move(value); }
        
        void unhandled_exception() { 
            exception = std::current_exception(); 
        }
    };

    explicit Task(std::coroutine_handle<promise_type> handle) 
        : handle_(handle) {}
    
    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    T result() const {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return handle_.promise().result;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

// Specialization for void
template <>
class Task<void> {
public:
    struct promise_type {
        std::exception_ptr exception;

        Task get_return_object() { 
            return Task(std::coroutine_handle<promise_type>::from_promise(*this)); 
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_void() {}
        
        void unhandled_exception() { 
            exception = std::current_exception(); 
        }
    };

    explicit Task(std::coroutine_handle<promise_type> handle) 
        : handle_(handle) {}
    
    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    void result() const {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

// Class to manage node information
class NodesMap {
public:
    NodesMap();
    ~NodesMap() = default;

    bool check_node(const HashIdentifier& node_id) const;
    bool check_info(const HashIdentifier& node_id, uint32_t info_id) const;
    bool check_heartbeat(const HashIdentifier& node_id, uint32_t info_id) const;
    
    void update_node(const HashIdentifier& node_id, const NodeInfo& node_info);
    void remove_node(const HashIdentifier& node_id);
    
    std::vector<SocketInfo> get_publisher_info(const TopicName& topic_name) const;
    SocketInfo get_service_info(const ServiceName& service_name) const;

private:
    std::unordered_map<HashIdentifier, NodeInfo> nodes_info_;
    std::unordered_map<HashIdentifier, uint32_t> nodes_info_id_;
    std::unordered_map<HashIdentifier, std::string> nodes_heartbeat_;
    std::unordered_map<HashIdentifier, SocketInfo> publishers_dict_;
    std::unordered_map<HashIdentifier, SocketInfo> services_dict_;
    
    mutable std::mutex mutex_;
};

// Abstract base class for all node types
class AbstractNode {
public:
    AbstractNode(const std::string& node_name, const IPAddress& node_ip);
    virtual ~AbstractNode();

    // Non-copyable
    AbstractNode(const AbstractNode&) = delete;
    AbstractNode& operator=(const AbstractNode&) = delete;
    
    // Start the node's event loops
    void spin();
    
    // Stop the node gracefully
    void stop_node();

    // Create a new ZMQ socket
    zmq::socket_t create_socket(int socket_type);
    
    // Submit a task to the event loop
    template <typename Func>
    auto submit_task(Func&& task, bool block = false) {
        auto future = std::async(std::launch::async, std::forward<Func>(task));
        
        if (block) {
            return future.get();
        }
        
        return future;
    }

protected:
    // Initialize the event loop with node-specific behavior
    virtual void initialize_event_loop() = 0;
    
    // Listen for multicast messages from other nodes
    Task<void> listen_loop();
    
    // Process received heartbeat messages
    Task<void> process_heartbeat(const BytesMessage& data, const IPAddress& ip);
    
    // Send a request to another node
    Task<BytesMessage> send_request(
        const std::string& service_name,
        const IPAddress& ip,
        Port port,
        const std::string& msg);

    // Node properties
    std::string node_name_;
    IPAddress node_ip_;
    
    // ZMQ context
    std::unique_ptr<zmq::context_t> zmq_context_;
    
    // Thread management
    std::atomic<bool> running_{false};
    std::vector<std::thread> threads_;
    
    // Node information tracking
    std::unique_ptr<NodesMap> nodes_map_;
};

} // namespace lancom
