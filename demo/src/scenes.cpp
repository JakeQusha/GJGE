#include "entity_management.hpp"
#include "generators.hpp"
#include "template.hpp"
#include "components/flags.hpp"
void generate_scenes(ge::SceneManager& scene_manager) {
    scene_manager.make_scene(
        "def",
        [](entt::registry& registry, ge::AssetManager&) {
            auto entity = ge::create(registry, "wojtek");
            ge::emplace<Dead>(registry, entity);
            ge::emplace<Alive>(registry, entity);
            entity = ge::create(registry, "static");
            ge::emplace<ge::comp::PersistentEntity>(registry, entity);
        },
        [](entt::registry&, ge::AssetManager&) {

        },
        [](entt::registry&, ge::AssetManager&) {

        });
    scene_manager.make_scene(
        "second",
        [](entt::registry& registry, ge::AssetManager&) {
            ge::instantiate(registry, "stary");
            ge::instantiate(registry, "stary");
        },
        [](entt::registry&, ge::AssetManager&) {

        },
        [](entt::registry&, ge::AssetManager&) {

        });
}
