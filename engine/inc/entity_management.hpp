#pragma once

#include <entt.hpp>
#include "gui/logs.hpp"

namespace ge {
template <typename T, typename... Args>
auto emplace(entt::registry& registry, entt::entity entity, const Args&... args) -> decltype(auto);

template <typename... T>
struct Dependencies {
    static void add_all([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
        (
            [&]() {
                if (!registry.all_of<T>(entity)) {
                    emplace<T>(registry, entity);
                }
            }(),
            ...);
    }
};

template <typename T>
concept HasDependencies = requires {
    { T::dependencies };
};

void kill(entt::entity entity);
void kill_unsafe(entt::registry& registry, entt::entity entity);
void empty_kill_queue(entt::registry& registry);
auto create(entt::registry& registry, const char* name) -> entt::entity;
auto create_child(entt::registry& registry,entt::entity parent, const char* name) -> entt::entity;
template <typename T, typename... Args>
auto emplace(entt::registry& registry, entt::entity entity, const Args&... args) -> decltype(auto){
    if constexpr (HasDependencies<T>) {
        T::dependencies.add_all(registry, entity);
    }
    return registry.emplace_or_replace<T>(entity, args...);
}

template <typename T>
void remove(entt::registry& registry, entt::entity entity) {
    registry.remove<T>(entity);
}

} // namespace ge
