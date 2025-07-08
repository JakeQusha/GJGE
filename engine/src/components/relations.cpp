#include "components/relations.hpp"
#include "entity_management.hpp"
#include <ranges>
#include "gui/logs.hpp"

auto ge::add_relation(entt::registry& registry, entt::entity parent, entt::entity child) -> bool {
    if (registry.all_of<comp::Child>(child)) {
        return false;
    }
    if (!registry.all_of<comp::Parent>(parent)) {
        registry.emplace<comp::Parent>(parent);
    }
    auto& child_comp = registry.emplace<comp::Child>(child);
    auto& parent_comp = registry.get<comp::Parent>(parent);
    child_comp.parent = parent;
    parent_comp.children.push_back(child);
    return true;
}

auto ge::remove_relation(entt::registry& registry, entt::entity parent, entt::entity child) -> bool {
    if (!registry.all_of<comp::Parent>(parent) || !registry.all_of<comp::Child>(child)) {
        return false;
    }
    auto& child_comp = registry.get<comp::Child>(child);
    if (child_comp.parent != parent) {
        return false;
    }
    auto& parent_comp = registry.get<comp::Parent>(parent);
    for (auto [i, x] : std::views::enumerate(parent_comp.children)) {
        if (x != child) {
            continue;
        }
        registry.remove<comp::Child>(child);
        std::swap(parent_comp.children.back(), parent_comp.children.at(static_cast<size_t>(i)));
        parent_comp.children.pop_back();
        if (parent_comp.children.empty()) {
            registry.erase<comp::Parent>(parent);
        }
        return true;
    }
    return false;
}

/// is called under the hood by the ge::remove function
void ge::relations_call_on_delete_entity(entt::registry& registry, entt::entity entity) {
    if (registry.all_of<comp::Parent>(entity)) {
        auto& parent = registry.get<comp::Parent>(entity);
        for (auto child : parent.children) {
            if (!registry.valid(child)) {
                continue;
            }
            registry.remove<comp::Child>(child);
            kill(registry, child);
        }
    }
    if (registry.all_of<comp::Child>(entity)) {
        const auto& child = registry.get<comp::Child>(entity);
        remove_relation(registry, child.parent, entity);
    }
}
