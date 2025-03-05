#include "lancom/log.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>

namespace lancom {

// Custom formatter for remote log level
class RemoteLogLevelFormatter : public spdlog::custom_flag_formatter {
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override {
        if (msg.level == spdlog::level::level_enum(Logger::REMOTE_LOG_LEVEL)) {
            dest.append("REMOTELOG");
        } else {
            const auto& level_str = spdlog::level::to_string_view(msg.level);
            dest.append(level_str.data(), level_str.data() + level_str.size());
        }
    }

    std::unique_ptr<custom_flag_formatter> clone() const override {
        return spdlog::details::make_unique<RemoteLogLevelFormatter>();
    }
};

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    // Register custom level
    spdlog::set_level(spdlog::level::debug);
    spdlog::register_level(REMOTE_LOG_LEVEL, "REMOTELOG");

    // Create color console logger
    logger_ = spdlog::stdout_color_mt("lancom");
    logger_->set_level(spdlog::level::debug);

    // Create custom formatter with colors
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<RemoteLogLevelFormatter>('l');
    formatter->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");
    logger_->set_formatter(std::move(formatter));
}

Logger::~Logger() {
    spdlog::drop_all();
}

void Logger::debug(const std::string& message) {
    logger_->debug(message);
}

void Logger::info(const std::string& message) {
    logger_->info(message);
}

void Logger::warning(const std::string& message) {
    logger_->warn(message);
}

void Logger::error(const std::string& message) {
    logger_->error(message);
}

void Logger::critical(const std::string& message) {
    logger_->critical(message);
}

void Logger::remote_log(const std::string& message) {
    logger_->log(spdlog::level::level_enum(REMOTE_LOG_LEVEL), message);
}

void Logger::set_level(int level) {
    logger_->set_level(spdlog::level::level_enum(level));
}

void Logger::set_pattern(const std::string& pattern) {
    logger_->set_pattern(pattern);
}

// Global functions
void debug(const std::string& message) {
    Logger::instance().debug(message);
}

void info(const std::string& message) {
    Logger::instance().info(message);
}

void warning(const std::string& message) {
    Logger::instance().warning(message);
}

void error(const std::string& message) {
    Logger::instance().error(message);
}

void critical(const std::string& message) {
    Logger::instance().critical(message);
}

void remote_log(const std::string& message) {
    Logger::instance().remote_log(message);
}

} // namespace lancom
