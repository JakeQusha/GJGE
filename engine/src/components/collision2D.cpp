#include "components/collision2D.hpp"
#include <rl.hpp>
#include <imgui.h>
#include <utility>
#include <vector>
#ifdef GJGE_DEV_TOOLS
void ge::draw_debug_colliders(entt::registry& registry) {
    auto view = registry.view<comp::AABBCollider, comp::Transform2D>();
    for (auto&& [entity, collider, transform] : view.each()) {
        auto corner = collider.get_corner(transform);
        auto size = collider.get_size(transform);
        auto center = collider.get_center(transform);
        rl::Rectangle(corner.x, corner.y, size.x, size.y).DrawLines(BLUE);
        DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), 2.f, RED);
    }
}
#endif

static auto aabb_overlap(entt::registry& registry, entt::entity first, entt::entity second) -> bool {
    auto&& [first_collider, first_transform] = registry.get<ge::comp::AABBCollider, ge::comp::Transform2D>(first);
    auto&& [second_collider, second_transform] = registry.get<ge::comp::AABBCollider, ge::comp::Transform2D>(second);
    const auto first_corner = first_collider.get_corner(first_transform);
    const auto first_size = first_collider.get_size(first_transform);
    const auto second_corner = second_collider.get_corner(second_transform);
    const auto second_size = second_collider.get_size(second_transform);
    return first_corner.x < second_corner.x + second_size.x && second_corner.x < first_corner.x + first_size.x &&
           first_corner.y < second_corner.y + second_size.y && second_corner.y < first_corner.y + first_size.y;
}

void ge::evaluate_AABB_Collisions(entt::registry& registry) {
    const auto view = registry.view<comp::AABBCollider, comp::Transform2D>();
    std::vector<entt::entity> entities;
    for (const auto entity : view) {
        entities.push_back(entity);
    }
    std::vector<std::pair<entt::entity, entt::entity>> collisions;
    for (size_t i = 0; i + 1 < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            if (aabb_overlap(registry, entities[i], entities[j])) {
                collisions.emplace_back(entities[i], entities[j]);
            }
        }
    }
    const auto has_collider = [&registry](entt::entity entity) {
        return registry.valid(entity) && registry.all_of<comp::AABBCollider>(entity);
    };
    for (const auto& [first, second] : collisions) {
        if (!has_collider(first) || !has_collider(second)) {
            continue;
        }
        if (const auto callback = registry.get<comp::AABBCollider>(first).on_collision_callback) {
            (*callback)(registry, first, second);
        }
        if (!has_collider(first) || !has_collider(second)) {
            continue;
        }
        if (const auto callback = registry.get<comp::AABBCollider>(second).on_collision_callback) {
            (*callback)(registry, second, first);
        }
    }
}

auto ge::comp::AABBCollider::get_center(const Transform2D& transform) const -> rl::Vector2 {
    return get_corner(transform) + get_size(transform) * 0.5f;
}
auto ge::comp::AABBCollider::get_corner(const Transform2D& transform) const -> rl::Vector2 {
    return {transform.global_position.x - (offset.x * size.x * std::abs(transform.global_scale.x)),
            transform.global_position.y - (offset.y * size.y * std::abs(transform.global_scale.y))};
}
auto ge::comp::AABBCollider::get_size(const Transform2D& transform) const -> rl::Vector2 {
    return {size.x * std::abs(transform.global_scale.x), size.y * std::abs(transform.global_scale.y)};
}
auto ge::comp::AABBCollider::get_intersection(const Transform2D& transform, const AABBCollider& other, const Transform2D& other_transform) const -> rl::Vector2 {
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
    return {first_corner.x + first_size.x /2 > second_corner.x + second_size.x /2 ? x : -x, first_corner.y + first_size.y /2 > second_corner.y + second_size.y /2 ? y : -y};
}
#ifdef GJGE_DEV_TOOLS
void ge::comp::AABBCollider::inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
    ImGui::DragFloat2("Offset:", &offset.x, 0.01f);
    ImGui::DragFloat2("Size:", &size.x, 1, 0);
}
#endif
