#pragma once

#include <functional>
#include "entt.hpp"
#include "assets/asset_manager.hpp"
#include "entity_management.hpp"

namespace ge {
    typedef std::function<void(entt::registry &, AssetManager &, entt::entity)> Recipe_t;
    struct Template {
        const char *name;
        Recipe_t recipe;
    };

    void make_template(entt::registry &registry, const char *name, Recipe_t &&recipie);

    entt::entity instantiate_template(entt::registry &registry, const char *name);
}