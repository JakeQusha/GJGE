#include "template.hpp"
#include "scene.hpp"
#include "entity_management.hpp"
#include "components/sprite.hpp"
#include "components/collision2D.hpp"
#include "components/relations.hpp"
#include <format>
void generate_templates(entt::registry& registry) {
    ge::make_template(registry, "stary",
                      [](entt::registry& registry, ge::AssetManager& asset_manager, entt::entity entity) {
                          auto&& [sprite, transform] = ge::emplace<ge::comp::Sprite>(registry, entity);
                          sprite.texture = asset_manager.get<ge::MultiTexture>("blue");
                          auto mlody = ge::create(registry, "mlody");
                          auto&& [sprite1, transform1] = ge::emplace<ge::comp::Sprite>(registry, mlody);
                          sprite1.texture = asset_manager.get<ge::MultiTexture>("orange");
                          auto niemowle = ge::create(registry, "mniemowle");
                          ge::add_relation(registry, mlody, niemowle);
                          ge::add_relation(registry, entity, mlody);
                      });
    ge::make_template(
        registry, "testowy kolider",
        [](entt::registry& registry, [[maybe_unused]] ge::AssetManager& asset_manager, entt::entity entity) {
            auto&& [col, transform] = ge::emplace<ge::comp::AABBCollider>(registry, entity);
            col.on_collision_callback = []([[maybe_unused]] entt::registry& reg, entt::entity main,
                                           entt::entity other) {
                ge::logger.log(ge::LogLevel::DEBUG, std::format("{} tycniety by {}", static_cast<uint32_t>(main),
                                                                    static_cast<uint32_t>(other))
                                                            .c_str());
            };
        });
}
