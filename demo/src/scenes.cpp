#include "entity_management.hpp"
#include "generators.hpp"
#include "template.hpp"
#include "components/flags.hpp"
void generate_scenes(const ge::SceneManager& scene_manager) {
    scene_manager.make_scene(
        "def",
        [](entt::registry& registry, ge::AssetManager&) {
            auto entity = ge::create(registry, "wojtek");
            registry.emplace<Dead>(entity);
            registry.emplace<Alive>(entity);
            entity = ge::create(registry, "static");
            registry.emplace<ge::comp::PersistentEntity>(entity);
        },
        [](entt::registry&, ge::AssetManager&) {

        },
        [](entt::registry&, ge::AssetManager&) {

        });
    scene_manager.make_scene(
        "second",
        [](entt::registry& registry, ge::AssetManager& asset_manager) {
            ge::instantiate_template(registry, "stary");
            ge::instantiate_template(registry, "stary");
        },
        [](entt::registry&, ge::AssetManager&) {

        },
        [](entt::registry&, ge::AssetManager&) {

        });
}
