#include "template.hpp"
#include "entity_management.hpp"
#include "components/relations.hpp"
void ge::make_template(entt::registry& registry, const char* name, Recipe_t&& recipe) {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    asset_manager.emplace<Template>(name, name, std::forward<Recipe_t>(recipe));
}

auto ge::instantiate(entt::registry& registry, const char* name) -> entt::entity {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    const auto& _template = asset_manager.get<Template>(name);
    const auto entity = create(registry, _template.name);
    _template.recipe(registry, asset_manager, entity);
    return entity;
}
auto ge::instantiate_child(entt::registry& registry, entt::entity parent, const char* template_name) -> entt::entity {
    auto entity = instantiate(registry,template_name);
    add_relation(registry,parent,entity);
    return entity;
}
