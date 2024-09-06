#pragma once

#include <functional>
#include "entt.hpp"
#include "assets/asset_manager.hpp"
#include "entity_management.hpp"

namespace ge {
    typedef std::function<void(entt::registry &, entt::entity)> Recipie_t;
    struct Template {
        const char *name;
        Recipie_t recipie;
    };

    void make_template(entt::registry &registry, const char *name, Recipie_t &&recipie) {
        auto &asset_manager = registry.ctx().get<AssetManager>();
        asset_manager.emplace<Template>(name, name, std::forward<Recipie_t &&>(recipie));
    }

    entt::entity instantiate_template(entt::registry &registry, const char *name) {
        auto &asset_manager = registry.ctx().get<AssetManager>();
        Template &_template = asset_manager.get<Template>(name);
        auto entity = create(registry, _template.name);
        _template.recipie(registry, entity);
        return entity;
    }
}