#include "template.hpp"

void ge::make_template(entt::registry& registry, const char* name, ge::Recipe_t&& recipie) {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    asset_manager.emplace<Template>(name, name, std::forward<Recipe_t&&>(recipie));
}

entt::entity ge::instantiate_template(entt::registry& registry, const char* name) {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    Template& _template = asset_manager.get<Template>(name);
    auto entity = create(registry, _template.name);
    _template.recipe(registry, asset_manager, entity);
    return entity;
}
