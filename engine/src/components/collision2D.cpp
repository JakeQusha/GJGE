#include "components/collision2D.hpp"
#include "raylib.h"

static Vector2 get_center(const ge::comp::Transform2D &transform, const ge::comp::AABBCollider &collider) {
    return {transform.global_position.x - collider.offset.x * collider.size.x * std::abs(transform.global_scale.x),
            transform.global_position.y - collider.offset.y * collider.size.y * std::abs(transform.global_scale.y)};
}

static Vector2 get_size(const ge::comp::Transform2D &transform, const ge::comp::AABBCollider &collider) {
    return {collider.size.x * std::abs(transform.global_scale.x), collider.size.y * std::abs(transform.global_scale.y)};
}

void ge::draw_debug_colliders(entt::registry &registry) {
    auto view = registry.view<comp::AABBCollider, comp::Transform2D>();
    for (auto &&[entity, collider, transform]: view.each()) {
        auto center = get_center(transform, collider);
        auto size = get_size(transform, collider);
        DrawRectangleLines(static_cast<int>(center.x), static_cast<int>(center.y),
                           static_cast<int>(size.x), static_cast<int>(size.y), RED);
    }
}

void ge::evaluate_AABB_Collisions(entt::registry &registry) {
    //auto view = registry.view<comp::Transform2D, comp::AABBCollider>();
    //TODO
}

void ge::comp::AABBCollider::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::DragFloat2("Offset:", &offset.x, 0.01f);
    ImGui::DragFloat2("Size:", &size.x, 1, 0);
    ImGui::Checkbox("Logical Only", &logical_only);
}
