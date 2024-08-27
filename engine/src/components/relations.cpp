#pragma once

#include "relations.hpp"
#include "entity_managment.hpp"
#include <ranges>

bool ge::comp::add_relation(entt::registry &registry, entt::entity parent, entt::entity child) {
    if (registry.all_of<Child>(child)) {
        return false;
    }
    if (!registry.all_of<Parent>(parent)) {
        registry.emplace<Parent>(parent);
    }
    registry.emplace<Child>(child);
    auto &child_comp = registry.get<Child>(child);
    auto &parent_comp = registry.get<Parent>(parent);
    child_comp.parent = parent;
    parent_comp.children.push_back(child);
    return true;
}

bool ge::comp::remove_relation(entt::registry &registry, entt::entity parent, entt::entity child) {
    if (!registry.all_of<Parent>(parent) || !registry.all_of<Child>(child)) {
        return false;
    }
    auto &child_comp = registry.get<Child>(child);
    if (child_comp.parent != parent) {
        return false;
    }
    auto &parent_comp = registry.get<Parent>(parent);
    for (auto [i, x]: std::views::enumerate(parent_comp.children)) {
        if (x != child) continue;
        registry.remove<Child>(child);
        parent_comp.children.erase(parent_comp.children.begin() + i);
        return true;
    }
    return false;
}

///is called under the hood by the ge::remove function
void ge::comp::relations_call_on_delete_entity(entt::registry &registry, entt::entity entity) {
    if (registry.all_of<Parent>(entity)) {
        auto &parent = registry.get<Parent>(entity);
        for (auto child: parent.children) {
            if (!registry.valid(child)) {
                continue;
            }
            registry.remove<Child>(child);
            kill(registry, child);
        }
    }
    if (registry.all_of<Child>(entity)) {
        auto &child = registry.get<Child>(entity);
        remove_relation(registry, child.parent, entity);
    }
}
