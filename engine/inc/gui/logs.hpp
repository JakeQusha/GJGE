#pragma once
#include <queue>
#include <cstdint>
#include <cstring>
namespace ge{
enum class LogLevel {
    ERR,
    INFO,
    DEBUG,
    NONE
};
struct Log {
    LogLevel ll;
    char *log;
    bool command_caused;

    Log() = delete;

    Log(const Log &) = delete;

    Log(Log &&other) noexcept: ll(other.ll), log(other.log), command_caused(other.command_caused) {
        other.log = nullptr;
    }

    Log(LogLevel log_level, const char *message, bool command_caused) : ll(log_level),
                                                                        command_caused(command_caused) {

        size_t message_len = strlen(message);
        log = new char[9 + message_len];
        uint8_t offset = 0;
        switch (ll) {
            case LogLevel::ERR:
                strcpy(log, "[Error] ");
                offset = 8;
                break;
            case LogLevel::NONE:
                break;
            case LogLevel::INFO:
                strcpy(log, "[Info] ");
                offset = 7;
                break;
            case LogLevel::DEBUG:
                strcpy(log, "[Debug] ");
                offset = 8;
                break;
        }
        strcpy(log + offset, message);
    }

    ~Log() {
        delete log;
    }
};

struct Logger{
    std::queue<Log> logs;
    void add_log(LogLevel ll, const char *message) {
        logs.emplace(ll, message, false);
    }
};
inline Logger logger;
}