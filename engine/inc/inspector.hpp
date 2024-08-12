#pragma once
#include <imgui.h>
#include <entt.hpp>

template<typename T>
concept InspectableComponent = requires(T t, entt::registry &registry, entt::entity entity) {
    { T::name } -> std::convertible_to<const char *>;
    { t.inspect(registry, entity) };
} || requires() {
    { T::name } -> std::convertible_to<const char *>;
    { T::inspect() };
    { std::is_empty_v<T> };
};

template<InspectableComponent T>
void inspect(T &t, entt::registry &registry, entt::entity entity) {
    if constexpr (std::is_empty_v<T>) {
        T::inspect();
    } else {
        t.inspect(registry, entity);
    }
}

template<InspectableComponent... Component>
struct Inspector {

    void draw_gui(){
        ImGui::Begin("Inspector");
    }
};