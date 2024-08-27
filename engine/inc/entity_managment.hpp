#pragma once

#include <entt.hpp>
#include "relations.hpp"
namespace ge{
    inline void kill(entt::registry &registry, entt::entity entity){
        if(!registry.valid(entity)){
            return;
        }
        comp::relations_call_on_delete_entity(registry,entity);
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
