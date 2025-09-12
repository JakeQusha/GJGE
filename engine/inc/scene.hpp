#pragma once
#include "entt.hpp"
#include "imgui.h"
#include "assets/asset_manager.hpp"
#include <functional>
#define GEN_SCENES(name) inline void name(ge::SceneManager& scene_manager)
#define SCENE(name)                                                                                                    \
    extern void gen_##name##_scene(const ge::SceneManager&);                                                           \
    gen_##name##_scene(scene_manager);
#define SCENE_DEF(name)                                                                                                \
    static void setup(entt::registry&, ge::AssetManager&);                                                             \
    static void update(entt::registry&, ge::AssetManager&);                                                            \
    static void clean(entt::registry&, ge::AssetManager&);                                                             \
    void gen_##name##_scene(const ge::SceneManager& scene_manager) {                                                   \
        scene_manager.make_scene(#name, setup, update, clean);                                                         \
    }
#define SETUP                                                                                                          \
    static void setup([[maybe_unused]] entt::registry& registry, [[maybe_unused]] ge::AssetManager& asset_manager)
#define UPDATE                                                                                                         \
    static void update([[maybe_unused]] entt::registry& registry, [[maybe_unused]] ge::AssetManager& asset_manager)
#define CLEAN                                                                                                          \
    static void clean([[maybe_unused]] entt::registry& registry, [[maybe_unused]] ge::AssetManager& asset_manager)

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
    Scene_recipe_t clean;
};
class SceneManager {
    Scene* current_scene = nullptr;
    entt::registry& registry;

public:
    bool freeze = false;
    explicit SceneManager(entt::registry& registry);

    void make_scene(const char* name, Scene_recipe_t&& setup, Scene_recipe_t&& update, Scene_recipe_t&& clean) const;

    void load_scene(const char* name);

    void unload_scene(bool destroy_persistent_entities = false);

    void tick() const;

    [[nodiscard]] auto get_current_scene() const -> const char*;
};

} // namespace ge