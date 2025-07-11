#pragma once

#include <functional>
#include "entt.hpp"
#include "assets/asset_manager.hpp"
#define GENERATE_TEMPLATES(name) inline void name(entt::registry& registry)
#define TEMPLATE(name) \
    extern void gen_##name##_template(entt::registry &); \
    gen_##name##_template(registry);
#define TEMPLATE_DEF(name) \
    static void recipe_##name(entt::registry & registry, [[maybe_unused]] ge::AssetManager & asset_manager, entt::entity entity); \
    void gen_##name##_template(entt::registry & registry) { ge::make_template(registry, #name, recipe_##name); } \
    static void recipe_##name(entt::registry & registry, [[maybe_unused]] ge::AssetManager & asset_manager, entt::entity entity)


namespace ge {
using Recipe_t = std::function<void (entt::registry &, AssetManager &, entt::entity)>;
struct Template {
    const char* name;
    Recipe_t recipe;
};

void make_template(entt::registry& registry, const char* name, Recipe_t&& recipe);

auto instantiate_template(entt::registry& registry, const char* name) -> entt::entity;
} // namespace ge