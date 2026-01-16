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

  static T *getInstancePtr()
  {
    if (!instance_)
    {
      throw std::logic_error("Singleton not initialized. Call init() first.");
    }
    return instance_;
  }

  static T &instance()
  {
    return *getInstancePtr();
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
    unique_instance_ = std::move(inst);
    instance_ = unique_instance_.get();
  }

  static void init(T *inst)
  {
    if (instance_)
    {
      throw std::logic_error("Singleton already initialized.");
    }
    instance_ = inst;
  }

protected:
  Singleton() = default;
  ~Singleton() = default;
  inline static T* instance_ = nullptr;
  inline static std::unique_ptr<T> unique_instance_ = nullptr;
};
}
