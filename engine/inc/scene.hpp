#pragma once
#include "entt.hpp"
#include "imgui.h"
#include "assets/asset_manager.hpp"
#include <functional>

namespace ge {
namespace comp {
struct PersistentEntity {
    static constexpr auto name = "Persistent Entity";
    static void inspect() { ImGui::Text("Entity is persistent between scenes"); }
};
} // namespace comp
using Scene_recipe_t = std::function<void(entt::registry&, AssetManager&)>;
struct Scene {
    const char* name;
    Scene_recipe_t setup;
    Scene_recipe_t update;
};
class SceneManager {
    Scene* current_scene = nullptr;
    entt::registry& registry;

public:
    SceneManager(entt::registry& registry);
    void make_scene(const char* name, Scene_recipe_t&& setup, Scene_recipe_t&& update) const;

    void load_scene(const char* name);

    void unload_scene(bool destroy_persistent_entities = false);

    void tick() const;

    const char* get_current_scene() const;
};

} // namespace ge