#pragma once
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

template <typename T> class Singleton
{
public:
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;
  Singleton(Singleton &&) = delete;
  Singleton &operator=(Singleton &&) = delete;

  static T &instance()
  {
    if (!instance_)
    {
      throw std::logic_error("Singleton not initialized. Call init() first.");
    }
    return *instance_;
  }

  static bool isInitialized()
  {
    return instance_ != nullptr;
  }

  static void init(std::unique_ptr<T> inst)
  {
    if (instance_)
    {
      throw std::logic_error("Singleton already initialized.");
    }
    instance_ = std::move(inst);
  }

protected:
  Singleton() = default;
  ~Singleton() = default;
  inline static std::unique_ptr<T> instance_ = nullptr;
};
