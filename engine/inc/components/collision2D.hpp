#pragma once

#include <raylib.h>
#include <imgui.h>
#include <entt.hpp>
#include "entity_management.hpp"
#include "components/transform2D.hpp"

namespace ge {
    namespace comp {
        struct AABBCollider {
            static constexpr Dependencies<Transform2D> dependencies{};
            static constexpr auto name = "AABBCollider";
            bool logical_only{false};
            Vector2 offset{.5,.5};
            Vector2 size{50,50};
            void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity);
        };
    }
    void evaluate_AABB_Collisions(entt::registry &registry);
    void draw_debug_colliders(entt::registry &registry);
}