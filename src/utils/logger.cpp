#include "zerolancom/utils/logger.hpp"

#include <ctime>
#include <filesystem>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace zlc
{

/* ================= Logger ================= */

void Logger::init(bool enable_file_logging, const std::string &log_dir)
{
  // Ensure initialization happens only once
  if (initialized_)
    return;

  spdlog::init_thread_pool(8192, 1);

  std::vector<spdlog::sink_ptr> sinks;

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  sinks.push_back(console_sink);

  if (enable_file_logging)
  {
    std::filesystem::create_directories(log_dir);

    std::time_t t = std::time(nullptr);
    char ts[32];
    std::strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", std::localtime(&t));

    std::string file_path = log_dir + "/output_" + ts + ".txt";

    auto file_sink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path, true);

    sinks.push_back(file_sink);
  }

  auto async_logger = std::make_shared<spdlog::async_logger>(
      "zlc_async", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::set_default_logger(async_logger);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

  initialized_ = true;
}

void Logger::shutdown()
{
  if (!initialized_)
    return;

  initialized_ = false;
  spdlog::shutdown();
}

bool Logger::isInitialized()
{
  return initialized_;
}

void Logger::setLevel(LogLevel lvl)
{
  spdlog::set_level(toSpdLevel(lvl));
}

spdlog::level::level_enum Logger::toSpdLevel(LogLevel lvl)
{
  switch (lvl)
  {
  case LogLevel::TRACE:
    return spdlog::level::trace;
  case LogLevel::INFO:
    return spdlog::level::info;
  case LogLevel::WARN:
    return spdlog::level::warn;
  case LogLevel::ERROR:
    return spdlog::level::err;
  case LogLevel::FATAL:
    return spdlog::level::critical;
  }
  return spdlog::level::info;
}

/* ================= free functions ================= */

void trace(const std::string &msg)
{
  if (Logger::isInitialized())
    spdlog::trace(msg);
}

void info(const std::string &msg)
{
  if (Logger::isInitialized())
    spdlog::info(msg);
}

void warn(const std::string &msg)
{
  if (Logger::isInitialized())
    spdlog::warn(msg);
}

void error(const std::string &msg)
{
  if (Logger::isInitialized())
    spdlog::error(msg);
}

void fatal(const std::string &msg)
{
  spdlog::critical(msg);
}

} // namespace zlc
