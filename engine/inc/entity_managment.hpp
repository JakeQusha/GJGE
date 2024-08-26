#pragma once

#include <entt.hpp>
namespace ge{
    template<typename T>
    void kill(entt::registry &registry, entt::entity entity){
        registry.destroy(entity);
    }
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
