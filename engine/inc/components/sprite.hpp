#pragma once

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <optional>
#include <entt.hpp>
#include "assets/texture.hpp"
#include "transform2D.hpp"
#include "gui/inspector_comp.hpp"
namespace ge::comp {
    struct Sprite {
        static constexpr auto name = "Sprite";
        std::optional<MultiTexture> texture = std::nullopt;
        uint16_t id = 0;
        Vector2 offset{0.5f, 0.5f};
        Color tint = WHITE;

        void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
            auto am = registry.ctx().get<ge::AssetManager>();
            display_asset_manager_picker<ge::MultiTexture>("Texture",texture,am);
            ImGui::DragFloat2("Offset", &offset.x, 0.01f);
            ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &id, 1.0f);
        }
    };

    void draw_sprites(entt::registry &registry) {
        auto view = registry.view<Sprite, Transform2D>();
        for (auto &&[entity, sprite, transform]: view.each()) {
            if (!sprite.texture) {
                continue;
            }
            auto &txt = sprite.texture.value();
            auto rect = txt.rect_multi(sprite.id);
            rect.width *= transform.global_scale.x < 0 ? -1 : 1;
            rect.height *= transform.global_scale.y < 0 ? -1 : 1;
            auto width = std::abs(rect.width) * std::abs(transform.global_scale.x);
            auto height = std::abs(rect.height) * std::abs(transform.global_scale.y);
            DrawTexturePro(txt.texture, rect, Rectangle{transform.global_position.x, transform.global_position.y, width, height},
                           Vector2Multiply(sprite.offset, Vector2(width, height)), RAD2DEG * transform.global_rotation,
                           sprite.tint);
        }
    }
}
