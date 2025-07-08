#include "entity_management.hpp"
#include "generators.hpp"
#include "template.hpp"
#include "components/flags.hpp"
#include "components/transform2D.hpp"
void generate_scenes(ge::SceneManager& scene_manager) {
    scene_manager.make_scene(
        "def",
        [](entt::registry& registry, ge::AssetManager& asset_manager) {
            auto entity = ge::create(registry, "wojtek");
            registry.emplace<Dead>(entity);
            registry.emplace<Alive>(entity);
            entity = ge::create(registry,"static");
            registry.emplace<ge::comp::PersistentEntity>(entity);
        },
        [](entt::registry& registry, ge::AssetManager& asset_manager) {

        });
    scene_manager.make_scene(
        "second",
        [](entt::registry& registry, ge::AssetManager& asset_manager) {
            ge::instantiate_template(registry, "stary");
            ge::instantiate_template(registry, "stary");
        },
        [](entt::registry& registry, ge::AssetManager& asset_manager) {

        });
}
