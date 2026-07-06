#include "components/transform2D.hpp"
#include "components/relations.hpp"
#include <rl.hpp>
#include <imgui.h>
#ifdef GJGE_DEV_TOOLS
void ge::comp::Transform2D::inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
    ImGui::Text("Global Position: (%.2f, %.2f)", global_position.x, global_position.y);
    ImGui::Text("Global Rotation: %.2f", global_rotation);
    ImGui::Text("Global Scale: (%.2f, %.2f)", global_scale.x, global_scale.y);
    ImGui::DragFloat2("Position", &position.x, 1.f);
    ImGui::DragFloat("Rotation", &rotation, 0.02f);
    ImGui::DragFloat2("Scale", &scale.x, 0.02f);
}
#endif

static void calculate_transform(entt::registry& registry, entt::entity entity, ge::comp::Transform2D& transform, // NOLINT(*-no-recursion)
                                entt::entity parent, bool force) {
    const bool dirty = transform.position.x != transform._last_position.x || transform.position.y != transform._last_position.y ||
                       transform.rotation != transform._last_rotation || transform.scale.x != transform._last_scale.x ||
                       transform.scale.y != transform._last_scale.y;
    if (force || dirty) {
        force = true;
        // cache the *local* values that the dirty check above compares against
        transform._last_position = transform.position;
        transform._last_rotation = transform.rotation;
        transform._last_scale = transform.scale;
        if (parent == entt::null) {
            transform.global_position = transform.position;
            transform.global_rotation = transform.rotation;
            transform.global_scale = transform.scale;
        } else {
            const auto& parent_transform = registry.get<ge::comp::Transform2D>(parent);
            transform.global_position = parent_transform.global_position + transform.position.Rotate(parent_transform.global_rotation);
            transform.global_rotation = parent_transform.global_rotation + transform.rotation;
            transform.global_scale = parent_transform.global_scale * transform.scale;
        }
    }

    const auto* parent_comp = registry.try_get<ge::comp::Parent>(entity);
    if (!parent_comp) {
        return;
    }
    for (auto child : parent_comp->children) {
        auto& child_transform = registry.get<ge::comp::Transform2D>(child);
        calculate_transform(registry, child, child_transform, entity, force);
    }
}

void ge::calculate_global_transform(entt::registry& registry) {
    auto view = registry.view<comp::Transform2D>();
    for (auto&& [entity, transform] : view.each()) {
        if (registry.all_of<comp::Child>(entity)) {
            continue;
        }
        calculate_transform(registry, entity, transform, entt::null, false);
    }
}
