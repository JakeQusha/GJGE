#include "template.hpp"
#include "entity_management.hpp"
void ge::make_template(entt::registry& registry, const char* name, Recipe_t&& recipe) {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    asset_manager.emplace<Template>(name, name, std::forward<Recipe_t&&>(recipe));
}

auto ge::instantiate_template(entt::registry& registry, const char* name) -> entt::entity {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    auto& _template = asset_manager.get<Template>(name);
    auto entity = create(registry, _template.name);
    _template.recipe(registry, asset_manager, entity);
    return entity;
}
