#pragma once

#include <entt.hpp>

namespace ge::comp {
    struct Parent {
        std::vector<entt::entity> children;
    };
    struct Child {
        entt::entity parent;
    };

bool add_relation(entt::registry& registry,entt::entity parent,entt::entity child);
bool remove_relation(entt::registry& registry,entt::entity parent,entt::entity child);
}
