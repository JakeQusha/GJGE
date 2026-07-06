#pragma once
#include <cstdint>
#include <queue>
#include <string>
namespace ge {
enum class LogLevel : uint8_t { ERR, INFO, DEBUG, NONE };
struct Log {
    LogLevel ll;
    std::string log;
    bool command_caused;

    Log(LogLevel log_level, const char* message, bool command_caused)
        : ll(log_level), log(std::string(prefix(log_level)) + message), command_caused(command_caused) {}

private:
    static auto prefix(LogLevel ll) -> const char* {
        switch (ll) {
        case LogLevel::ERR:
            return "[Error] ";
        case LogLevel::INFO:
            return "[Info] ";
        case LogLevel::DEBUG:
            return "[Debug] ";
        case LogLevel::NONE:
            break;
        }
        return "";
    }
};

struct Logger {
    static constexpr size_t max_logs = 2048;
    std::queue<Log> logs;
    void log(LogLevel ll, const char* message) {
        logs.emplace(ll, message, false);
        while (logs.size() > max_logs) {
            logs.pop();
        }
    }
};
inline Logger logger;
} // namespace ge
