#pragma once

#include <memory>
#include <string>

namespace spdlog {
    class logger;
}

namespace lancom {

class Logger {
public:
    static Logger& instance();

    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    void remote_log(const std::string& message);

    void set_level(int level);
    void set_pattern(const std::string& pattern);

    // Custom log level for remote logs (like in Python implementation)
    static constexpr int REMOTE_LOG_LEVEL = 25;

private:
    Logger();
    ~Logger();
    
    // Non-copyable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::shared_ptr<spdlog::logger> logger_;
};

// Global logger accessor functions
void debug(const std::string& message);
void info(const std::string& message);
void warning(const std::string& message);
void error(const std::string& message);
void critical(const std::string& message);
void remote_log(const std::string& message);

} // namespace lancom
