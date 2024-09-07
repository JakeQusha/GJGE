#include "templates.hpp"
#include "template.hpp"
#include "components/sprite.hpp"

void generate_templates(entt::registry &registry) {
    ge::make_template(registry, "stary",
                      [](entt::registry &registry, ge::AssetManager asset_manager, entt::entity entity) {
                          auto &sprite = registry.emplace<ge::comp::Sprite>(entity);
                          sprite.texture = asset_manager.get<ge::MultiTexture>("blue");
                          auto mlody = ge::create(registry, "mlody");
                          auto &sprite1 = registry.emplace<ge::comp::Sprite>(mlody);
                          sprite1.texture = asset_manager.get<ge::MultiTexture>("orange");
                          auto niemowle = ge::create(registry, "mniemowle");
                          ge::add_relation(registry, mlody, niemowle);
                          ge::add_relation(registry, entity, mlody);
                      });

}
