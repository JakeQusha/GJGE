#include "components/collision2D.hpp"
#include "raylib.h"

void ge::draw_debug_colliders(entt::registry &registry) {
    auto view = registry.view<comp::AABBCollider, comp::Transform2D>();
    for (auto &&[entity, collider, transform]: view.each()) {
        DrawRectangleLines(
                (int) (transform.global_position.x -
                       collider.offset.x * collider.size.x * std::abs(transform.global_scale.x)),
                (int) (transform.global_position.y -
                       collider.offset.y * collider.size.y * std::abs(transform.global_scale.y)),
                (int) (collider.size.x * std::abs(transform.global_scale.x)),
                (int) (collider.size.y * std::abs(transform.global_scale.y)),
                RED);
    }
}

void ge::comp::AABBCollider::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::DragFloat2("Offset:",&offset.x,0.01f);
    ImGui::DragFloat2("Size:",&size.x,1,0);
    ImGui::Checkbox("Logical Only",&logical_only);
}
