#include "template.hpp"
#include "scene.hpp"
#include "components/sprite.hpp"
#include "components/collision2D.hpp"
#include "components/relations.hpp"
void generate_templates(entt::registry& registry) {
    ge::make_template(registry, "stary",
                      [](entt::registry& registry, ge::AssetManager& asset_manager, entt::entity entity) {
                          auto& sprite = registry.emplace<ge::comp::Sprite>(entity);
                          sprite.texture = asset_manager.get<ge::MultiTexture>("blue");
                          auto mlody = ge::create(registry, "mlody");
                          auto& sprite1 = registry.emplace<ge::comp::Sprite>(mlody);
                          sprite1.texture = asset_manager.get<ge::MultiTexture>("orange");
                          auto niemowle = ge::create(registry, "mniemowle");
                          ge::add_relation(registry, mlody, niemowle);
                          ge::add_relation(registry, entity, mlody);
                      });
    ge::make_template(
        registry, "testowy kolider",
        [](entt::registry& registry, [[maybe_unused]] ge::AssetManager& asset_manager, entt::entity entity) {
            auto& col = registry.emplace<ge::comp::AABBCollider>(entity);
            col.on_collision_callback = []([[maybe_unused]] entt::registry& reg, entt::entity main,
                                           entt::entity other) {
                ge::logger.log(ge::LogLevel::DEBUG, std::format("{} tycniety by {}", static_cast<uint32_t>(main),
                                                                    static_cast<uint32_t>(other))
                                                            .c_str());
            };
        });
}

