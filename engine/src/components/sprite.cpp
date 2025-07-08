#include "components/sprite.hpp"
#include <raymath.h>
void ge::draw_sprites(entt::registry& registry) {
    auto view = registry.view<comp::Sprite, comp::Transform2D>();
    for (auto&& [entity, sprite, transform] : view.each()) {
        if (!sprite.texture) {
            continue;
        }
        auto& txt = sprite.texture.value();
        auto rect = txt.rect_multi(sprite.id);
        rect.width *= transform.global_scale.x < 0 ? -1 : 1;
        rect.height *= transform.global_scale.y < 0 ? -1 : 1;
        auto width = std::abs(rect.width) * std::abs(transform.global_scale.x);
        auto height = std::abs(rect.height) * std::abs(transform.global_scale.y);
        DrawTexturePro(
            txt.texture, rect, Rectangle{transform.global_position.x, transform.global_position.y, width, height},
            Vector2Multiply(sprite.offset, Vector2(width, height)), RAD2DEG * transform.global_rotation, sprite.tint);
    }
}
