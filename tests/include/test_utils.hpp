#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>

#include "zerolancom/nodes/zerolancom_node.hpp"
#include "zerolancom/utils/logger.hpp"

namespace zlc_test
{

/**
 * @brief Thread-safe test result holder for async callbacks
 *
 * Use this to capture results from async operations (pub/sub, service calls)
 * and synchronize test assertions.
 */
template <typename T> class AsyncResult
{
public:
  void set(const T &value)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    value_ = value;
    received_ = true;
    cv_.notify_all();
  }

  T get() const
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return value_;
  }

  bool received() const
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return received_;
  }

  bool wait_for(std::chrono::milliseconds timeout)
  {
    std::unique_lock<std::mutex> lock(mutex_);
    return cv_.wait_for(lock, timeout, [this] { return received_; });
  }

  void reset()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    value_ = T{};
    received_ = false;
  }

private:
  mutable std::mutex mutex_;
  std::condition_variable cv_;
  T value_{};
  bool received_ = false;
};

/**
 * @brief Counter for tracking callback invocations
 */
class CallCounter
{
public:
  void increment()
  {
    count_.fetch_add(1, std::memory_order_relaxed);
  }
  int count() const
  {
    return count_.load(std::memory_order_relaxed);
  }
  void reset()
  {
    count_.store(0, std::memory_order_relaxed);
  }

private:
  std::atomic<int> count_{0};
};

/**
 * @brief Generate unique names for test isolation
 */
inline std::string unique_name(const std::string &prefix)
{
  static std::atomic<int> counter{0};
  return prefix + "_" + std::to_string(counter.fetch_add(1));
}

/**
 * @brief Simple echo service handler for testing
 */
inline std::string echoServiceHandler(const std::string &request)
{
  zlc::info("Echo service received: {}", request);
  return request;
}

} // namespace zlc_test