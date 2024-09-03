#pragma once

#include <entt.hpp>
#include "components/relations.hpp"
namespace ge{
    void kill(entt::registry &registry, entt::entity entity);
    entt::entity create(entt::registry &registry, const char *name);
    namespace comp {

        template<typename T, typename... Args>
        void emplace(entt::registry &registry, entt::entity entity, const Args &...args) {
            registry.emplace<T>(entity, args...);
        }

        template<typename T>
        void remove(entt::registry &registry, entt::entity entity) {
            registry.remove<T>(entity);
        }

    }
}
