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
    Vector2 offset{.5, .5};
    Vector2 size{50, 50};
    std::optional<std::function<void(entt::registry&, entt::entity, entt::entity)>> on_collision_callback;
    [[nodiscard]] auto get_center(const Transform2D& transform) const -> Vector2;
    [[nodiscard]] auto get_corner(const Transform2D& transform) const -> Vector2;
    [[nodiscard]] auto get_size(const Transform2D& transform) const -> Vector2;
    [[nodiscard]] auto get_intersection(const Transform2D& transform, const AABBCollider& other, const Transform2D& other_transform) const -> Vector2;
    void inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity);
};
} // namespace comp

void evaluate_AABB_Collisions(entt::registry& registry);

void draw_debug_colliders(entt::registry& registry);
} // namespace ge