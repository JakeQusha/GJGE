#include "scene.hpp"
#include "entity_management.hpp"
#include "components/relations.hpp"
#include "error.hpp"
#include <format>
#include <utility>
#include <vector>

ge::SceneManager::SceneManager(entt::registry& registry) : registry(registry) {}

void ge::SceneManager::make_scene(const char* name, Scene_recipe_t&& setup, Scene_recipe_t&& update, Scene_recipe_t&& clean) {
    auto& asset_manager = registry.ctx().get<AssetManager>();
    if (asset_manager.get_ptr<Scene>(name) != nullptr) {
        fatal_error(std::format("Scene '{}' is already registered", name));
    }
    asset_manager.emplace<Scene>(name, name, std::move(setup), std::move(update), std::move(clean));
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
    std::vector<entt::entity> to_destroy;
    for (const auto entity : registry.view<entt::entity>()) {
        if ((!destroy_persistent_entities && registry.all_of<comp::PersistentEntity>(entity)) ||
            registry.all_of<comp::Child>(entity)) {
            continue;
        }
        to_destroy.push_back(entity);
    }
    for (const auto entity : to_destroy) {
        kill_unsafe(registry, entity);
    }
    current_scene = nullptr;
}
void ge::SceneManager::tick() const {
    if (current_scene == nullptr) {
        return;
    }
#ifdef GJGE_DEV_TOOLS
    if (freeze) {
        return;
    }
#endif
    current_scene->update(registry, registry.ctx().get<AssetManager>());
}
auto ge::SceneManager::get_current_scene() const -> const char* {
    if (current_scene == nullptr) {
        return "None";
    }
    return current_scene->name;
}
