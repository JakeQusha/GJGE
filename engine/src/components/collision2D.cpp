#include "components/collision2D.hpp"
#include "raylib_operators.hpp"
#include "raylib.h"
#include <imgui.h>
void ge::draw_debug_colliders(entt::registry& registry) {
    auto view = registry.view<comp::AABBCollider, comp::Transform2D>();
    for (auto&& [entity, collider, transform] : view.each()) {
        auto corner = collider.get_corner(transform);
        auto size = collider.get_size(transform);
        auto center = collider.get_center(transform);
        DrawRectangleLines(static_cast<int>(corner.x), static_cast<int>(corner.y), static_cast<int>(size.x), static_cast<int>(size.y), BLUE);
        DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), 2.f, RED);
    }
}

static void process_AABBCollision(entt::registry& registry, entt::entity first, entt::entity second) {
    auto&& [first_collider, first_transform] = registry.get<ge::comp::AABBCollider, ge::comp::Transform2D>(first);
    auto&& [second_collider, second_transform] = registry.get<ge::comp::AABBCollider, ge::comp::Transform2D>(second);
    const auto first_corner = first_collider.get_corner(first_transform);
    const auto first_size = first_collider.get_size(first_transform);
    const auto second_corner = second_collider.get_corner(second_transform);
    const auto second_size = second_collider.get_size(second_transform);
    if (first_corner.x < second_corner.x + second_size.x && second_corner.x < first_corner.x + first_size.x &&
        first_corner.y < second_corner.y + second_size.y && second_corner.y < first_corner.y + first_size.y) {
        if (first_collider.on_collision_callback) {
            (*first_collider.on_collision_callback)(registry, first, second);
        }
        if (second_collider.on_collision_callback) {
            (*second_collider.on_collision_callback)(registry, second, first);
        }
    }

}

void ge::evaluate_AABB_Collisions(entt::registry& registry) {
    auto view = registry.view<comp::AABBCollider>();
    if (view.empty()) {
        return;
    }
    for (auto i = 0u; i < view.size() - 1; i++) {
        for (auto j = i + 1; j < view.size(); ++j) {
            process_AABBCollision(registry, view.begin()[i], view.begin()[j]);
        }
    }
}

auto ge::comp::AABBCollider::get_center(const Transform2D& transform) const -> Vector2 { return get_corner(transform) + get_size(transform) / 2; }
auto ge::comp::AABBCollider::get_corner(const Transform2D& transform) const -> Vector2 {
    return {transform.global_position.x - (offset.x * size.x * std::abs(transform.global_scale.x)),
            transform.global_position.y - (offset.y * size.y * std::abs(transform.global_scale.y))};
}
auto ge::comp::AABBCollider::get_size(const Transform2D& transform) const -> Vector2 {
    return {size.x * std::abs(transform.global_scale.x), size.y * std::abs(transform.global_scale.y)};
}
auto ge::comp::AABBCollider::get_intersection(const Transform2D& transform, const AABBCollider& other, const Transform2D& other_transform) const -> Vector2 {
    const auto first_corner = get_corner(transform);
    const auto first_size = get_size(transform);
    const auto second_corner = other.get_corner(other_transform);
    const auto second_size = other.get_size(other_transform);
    auto x = std::min(first_corner.x, second_corner.x) + (first_corner.x < second_corner.x ? first_size.x : second_size.x) -
              std::max(first_corner.x, second_corner.x);
    auto y = std::min(first_corner.y, second_corner.y) + (first_corner.y < second_corner.y ? first_size.y : second_size.y) -
              std::max(first_corner.y, second_corner.y);
    x = (x < first_size.x ? x : 0.f);
    y = (y < first_size.y ? y : 0.f);
    return {first_corner.x + first_size.x /2 > second_corner.x + second_size.x /2 ? x : -x, first_corner.y + first_size.y /2 > second_corner.y + first_size.x /2 ? y : -y};
}

void ge::comp::AABBCollider::inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
    ImGui::DragFloat2("Offset:", &offset.x, 0.01f);
    ImGui::DragFloat2("Size:", &size.x, 1, 0);
}
