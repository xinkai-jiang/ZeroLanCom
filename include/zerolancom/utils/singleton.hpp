#pragma once
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace zlc
{
template <typename T> class Singleton
{
public:
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;
  Singleton(Singleton &&) = delete;
  Singleton &operator=(Singleton &&) = delete;

  static void destroy()
  {
    if (unique_instance_)
    {
      unique_instance_.reset();
      instance_ = nullptr;
    }
    else if (instance_)
    {
      delete instance_;
      instance_ = nullptr;
    }
  }

  static T *instancePtr()
  {
    if (!instance_)
    {
      throw std::logic_error("Singleton not initialized. Call init() first.");
    }
    return instance_;
  }

  static T &instance()
  {
    return *instancePtr();
  }

  static bool isInitialized()
  {
    return instance_ != nullptr;
  }

  template <typename... Args> static void initManaged(Args &&...args)
  {
    if (instance_)
    {
      throw std::logic_error("Singleton already initialized.");
    }
    unique_instance_ = std::make_unique<T>(std::forward<Args>(args)...);
    instance_ = unique_instance_.get();
  }

  template <typename... Args> static void initExternal(Args &&...args)
  {
    if (instance_)
    {
      throw std::logic_error("Singleton already initialized.");
    }
    instance_ = new T(std::forward<Args>(args)...);
  }

protected:
  Singleton() = default;
  ~Singleton() = default;
  inline static T *instance_ = nullptr;
  inline static std::unique_ptr<T> unique_instance_ = nullptr;
};
} // namespace zlc
