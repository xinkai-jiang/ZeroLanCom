#pragma once

#include <functional>
#include <mutex>
#include <vector>

namespace zlc
{

/**
 * @brief Simple event class for managing callbacks.
 *
 * Thread-safe event handler that allows multiple subscribers.
 * Subscribers are invoked in the order they were registered.
 *
 * @tparam Args Callback argument types
 */
template <typename... Args> class Event
{
public:
  using CallbackType = std::function<void(Args...)>;

  /**
   * @brief Subscribe to the event.
   * @param callback Function to be called when event is triggered
   */
  void subscribe(const CallbackType &callback)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.push_back(callback);
  }

  /**
   * @brief Subscribe to the event (move version).
   * @param callback Function to be called when event is triggered
   */
  void subscribe(CallbackType &&callback)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.push_back(std::move(callback));
  }

  /**
   * @brief Trigger the event, calling all subscribed callbacks.
   * @param args Arguments to pass to the callbacks
   */
  void trigger(Args... args) const
  {
    std::vector<CallbackType> callbacks_copy;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      callbacks_copy = callbacks_;
    }

    for (const auto &callback : callbacks_copy)
    {
      callback(args...);
    }
  }

  /**
   * @brief Clear all subscribed callbacks.
   */
  void clear()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_.clear();
  }

  /**
   * @brief Get the number of subscribed callbacks.
   * @return Number of subscribers
   */
  size_t subscriberCount() const
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return callbacks_.size();
  }

private:
  mutable std::mutex mutex_;
  std::vector<CallbackType> callbacks_;
};

} // namespace zlc
