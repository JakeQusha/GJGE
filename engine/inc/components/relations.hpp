#pragma once
#include <entt.hpp>
#include <imgui.h>

namespace ge {
    namespace comp {
        struct Parent {
            std::vector <entt::entity> children;
        };
        struct Child {
            entt::entity parent;
        };
    }

    bool add_relation(entt::registry &registry, entt::entity parent, entt::entity child);

    bool remove_relation(entt::registry &registry, entt::entity parent, entt::entity child);

    void relations_call_on_delete_entity(entt::registry &registry, entt::entity entity);
}
