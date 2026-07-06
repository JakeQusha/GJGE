#pragma once
#ifdef GJGE_DEV_CONSOLE
#include "console.hpp"
#include <raylib.h>
#include <charconv>
#include <format>
#include <entt.hpp>
namespace ge {
struct hi_command {
    static constexpr auto name = "hi";
    static void execute([[maybe_unused]] Params& params, [[maybe_unused]] entt::registry& registry, LogCallback add_log) {
        add_log(LogLevel::NONE, "Hello World");
    }
};
struct echo_command {
    static constexpr auto name = "echo";
    static void execute([[maybe_unused]] const Params& params, [[maybe_unused]] entt::registry& registry, LogCallback add_log) {
        if (params.empty()) {
            return;
        }
        add_log(LogLevel::NONE, params.at(0).c_str());
    }
};
struct fps_command {
    static constexpr auto name = "fps";
    static void execute([[maybe_unused]] const Params& params, [[maybe_unused]] entt::registry& registry, LogCallback add_log) {
        if (params.empty()) {
            return;
        }
        const auto& param = params.at(0);
        int fps = 0;
        const auto [ptr, ec] = std::from_chars(param.data(), param.data() + param.size(), fps);
        if (ec != std::errc{} || ptr != param.data() + param.size() || fps < 0) {
            add_log(LogLevel::ERR, std::format("Invalid fps value: {}", param).c_str());
            return;
        }
        SetTargetFPS(fps);
        add_log(LogLevel::INFO, std::format("Set fps to {}", fps).c_str());
    }
};

} // namespace ge
#endif
