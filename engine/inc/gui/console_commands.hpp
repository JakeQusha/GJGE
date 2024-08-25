#pragma once
#include "console.hpp"
#include <entt.hpp>
namespace ge{
    struct hi_command{
        static constexpr auto name = "hi";
        static void execute([[maybe_unused]]Params &params,[[maybe_unused]]entt::registry &registry,LogCallback add_log){
            add_log(LogLevel::NONE,"Hello World");
        }
    };
    struct echo_command{
        static constexpr auto name = "echo";
        static void execute([[maybe_unused]]Params &params,[[maybe_unused]]entt::registry &registry,LogCallback add_log){
            add_log(LogLevel::NONE,params.at(0).c_str());
        }
    };

}