#pragma once
#ifdef GJGE_DEV_CONSOLE
#include "console.hpp"
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
        SetTargetFPS(std::stoi(params.at(0)));
        add_log(LogLevel::INFO, std::format("Set fps to {}", params.at(0)).c_str());
    }
};

} // namespace ge
#endif