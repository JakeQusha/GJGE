#include "scene.hpp"
#include "entity_management.hpp"
#include "components/relations.hpp"

ge::SceneManager::SceneManager(entt::registry& registry) : registry(registry) {}

void ge::SceneManager::make_scene(const char* name, Scene_recipe_t&& setup, Scene_recipe_t&& update,Scene_recipe_t&& clean) const {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    asset_manager.emplace<Scene>(name, name, std::forward<Scene_recipe_t>(setup), std::forward<Scene_recipe_t>(update), std::forward<Scene_recipe_t>(clean));
}
void ge::SceneManager::load_scene(const char* name) {
    unload_scene(false);
    auto& asset_manager = registry.ctx().get<AssetManager>();
    auto& scene = asset_manager.get<Scene>(name);
    scene.setup(registry, asset_manager);
    current_scene = &scene;
}
void ge::SceneManager::unload_scene(const bool destroy_persistent_entities) {
    if (current_scene) {
        current_scene->clean(registry, registry.ctx().get<AssetManager>());
    }
    for (const auto entity : registry.view<entt::entity>()) {
        if ((!destroy_persistent_entities && registry.all_of<comp::PersistentEntity>(entity)) ||
            registry.all_of<comp::Child>(entity)) {
            continue;
        }
        kill(registry, entity);
    }
    current_scene = nullptr;
}
void ge::SceneManager::tick() const {
    if (current_scene != nullptr) {
        current_scene->update(registry, registry.ctx().get<AssetManager>());
    }
}
const char* ge::SceneManager::get_current_scene() const {
    if (current_scene == nullptr) {
        return "None";
    }
    return current_scene->name;

}
