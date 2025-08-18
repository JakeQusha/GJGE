#pragma once

#include <entt.hpp>
#include "gui/logs.hpp"

namespace ge {
template <typename T, typename... Args>
auto safe_emplace(entt::registry& registry, entt::entity entity, const Args&... args) -> decltype(auto);

template <typename... T>
struct Dependencies {
    static void add_all([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
        (
            [&]() {
                if (!registry.all_of<T>(entity)) {
                    safe_emplace<T>(registry, entity);
                }
            }(),
            ...);
    }
    template <typename C>
    static auto build_tuple(entt::registry& registry, entt::entity entity, C& c) -> decltype(auto) {
        return std::tie(c, registry.get<T>(entity)...);
    }
};

template <typename T>
concept HasDependencies = requires {
    { T::dependencies.add_all };
};

void kill(entt::entity entity);
void kill_unsafe(entt::registry& registry, entt::entity entity);
void empty_kill_queue(entt::registry& registry);
auto create(entt::registry& registry, const char* name) -> entt::entity;
auto create_child(entt::registry& registry, entt::entity parent, const char* name) -> entt::entity;
template <typename T>
auto get(entt::registry& registry, entt::entity entity) -> decltype(auto) {
    if constexpr (HasDependencies<T>) {
        return T::dependencies.build_tuple(registry, entity, registry.get<T>(entity));
    } else {
        return registry.get<T>(entity);
    }
}
template <typename T, typename... Args>
auto safe_emplace(entt::registry& registry, entt::entity entity, const Args&... args) -> decltype(auto) {
    if (registry.all_of<T>(entity)) {
        return get<T>(registry, entity);
    }
    if constexpr (HasDependencies<T>) {
        T::dependencies.add_all(registry, entity);
        return T::dependencies.build_tuple(registry, entity, registry.emplace<T>(entity, args...));
    } else {
        return registry.emplace<T>(entity, args...);
    }
}
template <typename T, typename... Args>
auto emplace(entt::registry& registry, entt::entity entity, const Args&... args) -> decltype(auto) {
    if constexpr (HasDependencies<T>) {
        T::dependencies.add_all(registry, entity);
        return T::dependencies.build_tuple(registry, entity, registry.emplace_or_replace<T>(entity, args...));
    } else {
        return registry.emplace_or_replace<T>(entity, args...);
    }
}

template <typename T>
void remove(entt::registry& registry, entt::entity entity) {
    registry.remove<T>(entity);
}

} // namespace ge
