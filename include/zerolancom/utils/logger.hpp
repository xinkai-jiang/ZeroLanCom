#pragma once
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <string>

namespace zlc
{

// =======================
// Log levels
// =======================

enum class LogLevel
{
  TRACE = 0,
  INFO,
  WARN,
  ERROR,
  FATAL
};

// =======================
// Logger core
// =======================

class Logger
{
public:
  static void init(bool enable_file_logging = false,
                   const std::string &log_dir = "logs");

  static void shutdown();

  static bool isInitialized();

  static void setLevel(LogLevel lvl);

private:
  static spdlog::level::level_enum toSpdLevel(LogLevel lvl);
  static inline bool initialized_ = false;
};

// =======================
// Non-template helpers (defined in cpp)
// =======================

void trace(const std::string &msg);
void info(const std::string &msg);
void warn(const std::string &msg);
void error(const std::string &msg);
void fatal(const std::string &msg);

// =======================
// Template helpers (header-only, REQUIRED)
// =======================

template <typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args &&...args)
{
  if (Logger::isInitialized())
    spdlog::trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args &&...args)
{
  if (Logger::isInitialized())
    spdlog::info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args &&...args)
{
  if (Logger::isInitialized())
    spdlog::warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args &&...args)
{
  if (Logger::isInitialized())
    spdlog::error(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void fatal(fmt::format_string<Args...> fmt, Args &&...args)
{
  if (Logger::isInitialized())
    spdlog::critical(fmt, std::forward<Args>(args)...);
}

} // namespace zlc
