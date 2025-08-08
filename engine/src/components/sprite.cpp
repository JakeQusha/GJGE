#include "components/sprite.hpp"
#include <raymath.h>
void ge::comp::Sprite::inspect(entt::registry& registry, entt::entity entity) {
    auto& am = registry.ctx().get<AssetManager>();
    display_asset_manager_picker<MultiTexture>("Texture", texture, am);
    ImGui::DragFloat2("Offset", &offset.x, 0.01f);
    ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &id, 1.0f);
    ImGui::DragScalar("Layer",ImGuiDataType_S8, &layer, 1.0f);
}
void ge::draw_sprites(entt::registry& registry) {
    registry.sort<comp::Sprite>([](const comp::Sprite& lhs, const comp::Sprite& rhs) { return lhs.layer < rhs.layer; });
    auto view = registry.view<const comp::Sprite, comp::Transform2D>();
    view.use<comp::Sprite>();
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
        DrawTexturePro(txt.texture, rect, Rectangle{std::round(transform.global_position.x), std::round(transform.global_position.y), width, height},
                       Vector2Multiply(sprite.offset, Vector2(width, height)), RAD2DEG * transform.global_rotation, sprite.tint);
    }
}
