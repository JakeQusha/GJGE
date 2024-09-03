#include "components/transform2D.hpp"
#include "components/relations.hpp"
#include "raylib.h"
#include "raymath.h"

void ge::comp::Transform2D::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::Text("Global Position: (%.2f, %.2f)", global_position.x, global_position.y);
    ImGui::Text("Global Rotation: %.2f", global_rotation);
    ImGui::Text("Global Scale: (%.2f, %.2f)", global_scale.x, global_scale.y);
    ImGui::DragFloat2("Position", &position.x, 1.f);
    ImGui::DragFloat("Rotation", &rotation, 0.02f);
    ImGui::DragFloat2("Scale", &scale.x, 0.02f);
}

static void calculate_transform(entt::registry &registry, entt::entity entity, ge::comp::Transform2D &transform,
                                entt::entity parent,bool force) {
    if (!force && transform._last_position.x == transform.position.x && transform._last_position.y == transform.position.y &&
        transform.rotation == transform._last_rotation && transform.scale.x == transform._last_scale.x &&
        transform.scale.y == transform._last_scale.y) {
        goto next;
    }
    force = true;
    if (parent == entt::null) {
        transform.global_position = transform.position;
        transform.global_rotation = transform.rotation;
        transform.global_scale = transform.scale;
        goto next;
    }
    {
        auto &parent_transform = registry.get<ge::comp::Transform2D>(parent);
        transform._last_position = transform.global_position = Vector2Add(parent_transform.global_position,
                                                                          Vector2Rotate(transform.position,
                                                                                        parent_transform.global_rotation));
        transform._last_rotation = transform.global_rotation = parent_transform.global_rotation + transform.rotation;
        transform._last_scale = transform.global_scale = Vector2Multiply(parent_transform.global_scale,
                                                                         transform.scale);
    }
    next:

    auto *parent_comp = registry.try_get<ge::comp::Parent>(entity);
    if (!parent_comp) {
        return;
    }
    for (auto child: parent_comp->children) {
        auto &child_transform = registry.get<ge::comp::Transform2D>(child);
        calculate_transform(registry, child, child_transform, entity, force);
    }
}

void ge::calculate_global_transform(entt::registry &registry) {
    auto view = registry.view<comp::Transform2D>();
    for (auto &&[entity, transform]: view.each()) {
        if (registry.all_of<ge::comp::Child>(entity)) {
            continue;
        }
        calculate_transform(registry, entity, transform, entt::null, false);
    }
}
