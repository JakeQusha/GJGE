#pragma once

#include <functional>
#include "entt.hpp"
#include "assets/asset_manager.hpp"

namespace ge {
using Recipe_t = std::function<void (entt::registry &, AssetManager &, entt::entity)>;
struct Template {
    const char* name;
    Recipe_t recipe;
};

void make_template(entt::registry& registry, const char* name, Recipe_t&& recipe);

auto instantiate_template(entt::registry& registry, const char* name) -> entt::entity;
} // namespace ge