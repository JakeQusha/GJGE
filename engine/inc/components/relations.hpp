#pragma once
#include <entt.hpp>
#include <imgui.h>

namespace ge {
namespace comp {
struct Parent {
    std::vector<entt::entity> children;
};
struct Child {
    entt::entity parent;
};
} // namespace comp

auto add_relation(entt::registry& registry, entt::entity parent, entt::entity child) -> bool;

auto remove_relation(entt::registry& registry, entt::entity parent, entt::entity child) -> bool;

void relations_call_on_delete_entity(entt::registry& registry, entt::entity entity);
} // namespace ge
