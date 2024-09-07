#pragma once

#include <entt.hpp>
#include "gui/logs.hpp"
#include "components/relations.hpp"

namespace ge {
    template<typename T, typename... Args>
    void emplace(entt::registry &registry, entt::entity entity, const Args &...args);

    template<typename ...T>
    struct Dependencies {
        void add_all([[maybe_unused]]entt::registry &registry, [[maybe_unused]] entt::entity entity) const {
            ([&]() {
                if (!registry.all_of<T>(entity)) {
                    emplace<T>(registry, entity);
                }
            }, ...);
        }
    };

    template<typename T>
    concept HasDependencies = requires {
        { T::dependencies };
    };

    void kill(entt::registry &registry, entt::entity entity);

    entt::entity create(entt::registry &registry, const char *name);


    template<typename T, typename... Args>
    void emplace(entt::registry &registry, entt::entity entity, const Args &...args) {
        if constexpr (HasDependencies<T>) {
            T::dependencies.add_all(registry, entity);
        }
        registry.emplace_or_replace<T>(entity, args...);
    }


    template<typename T>
    void remove(entt::registry &registry, entt::entity entity) {
        registry.remove<T>(entity);
    }


}
