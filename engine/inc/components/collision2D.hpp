#pragma once

#include <raylib.h>
#include <entt.hpp>
#include "entity_management.hpp"
#include "components/transform2D.hpp"

namespace ge {
namespace comp {
struct AABBCollider {
    static constexpr Dependencies<Transform2D> dependencies{};
    static constexpr auto name = "AABBCollider";
    bool trigger_only = false;
    bool static_body = true;
    Vector2 offset{.5, .5};
    Vector2 size{50, 50};
    std::optional<std::function<void(entt::registry&, entt::entity, entt::entity)>> on_collision_callback;

    void inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity);
};
} // namespace comp

void evaluate_AABB_Collisions(entt::registry& registry);

void draw_debug_colliders(entt::registry& registry);
} // namespace ge