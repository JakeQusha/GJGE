#include "components/collision2D.hpp"
#include "raylib.h"
#include <imgui.h>

static auto get_center(const ge::comp::Transform2D& transform, const ge::comp::AABBCollider& collider) -> Vector2 {
    return {transform.global_position.x - (collider.offset.x * collider.size.x * std::abs(transform.global_scale.x)),
            transform.global_position.y - (collider.offset.y * collider.size.y * std::abs(transform.global_scale.y))};
}

static auto get_size(const ge::comp::Transform2D& transform, const ge::comp::AABBCollider& collider) -> Vector2 {
    return {collider.size.x * std::abs(transform.global_scale.x), collider.size.y * std::abs(transform.global_scale.y)};
}

void ge::draw_debug_colliders(entt::registry& registry) {
    auto view = registry.view<comp::AABBCollider, comp::Transform2D>();
    for (auto&& [entity, collider, transform] : view.each()) {
        auto center = get_center(transform, collider);
        auto size = get_size(transform, collider);
        DrawRectangleLines(static_cast<int>(center.x), static_cast<int>(center.y), static_cast<int>(size.x),
                           static_cast<int>(size.y), BLUE);
    }
}

static void process_AABBCollision(entt::registry& registry, entt::entity first, entt::entity second) {
    auto&& [first_collider, first_transform] = registry.get<ge::comp::AABBCollider, ge::comp::Transform2D>(first);
    auto&& [second_collider, second_transform] = registry.get<ge::comp::AABBCollider, ge::comp::Transform2D>(second);
    const auto first_center = get_center(first_transform, first_collider);
    const auto first_size = get_size(first_transform, first_collider);
    const auto second_center = get_center(second_transform, second_collider);
    const auto second_size = get_size(second_transform, second_collider);
    if (first_center.x < second_center.x + second_size.x && second_center.x < first_center.x + first_size.x &&
        first_center.y < second_center.y + second_size.y && second_center.y < first_center.y + first_size.y) {
        if (first_collider.on_collision_callback) {
            (*first_collider.on_collision_callback)(registry, first, second);
        }
        if (second_collider.on_collision_callback) {
            (*second_collider.on_collision_callback)(registry, second, first);
        }
        if (first_collider.logical_only || second_collider.logical_only) {
            return;
        }
        /*TODO PHYSIC STUFF*/
        return;
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

void ge::comp::AABBCollider::inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
    ImGui::DragFloat2("Offset:", &offset.x, 0.01f);
    ImGui::DragFloat2("Size:", &size.x, 1, 0);
    ImGui::Checkbox("Logical Only", &logical_only);
}
