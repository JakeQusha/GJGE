#pragma once

#include <raylib.h>
#include <entt.hpp>
#include <imgui.h>

namespace ge::comp {
    struct Transform2D {
        static constexpr auto name = "Transform2D";
        Vector2 position{0, 0};
        float rotation{.0f};
        Vector2 scale{1, 1};

        void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
    };
}