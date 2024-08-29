#pragma once

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <optional>
#include <entt.hpp>
#include "texture.hpp"
#include "transform2D.hpp"

namespace ge::comp {
    struct Sprite {
        static constexpr auto name = "Sprite";
        std::optional<MultiTexture> texture = std::nullopt;
        uint16_t id = 0;
        Vector2 offset{0.5f, 0.5f};
        Color tint = WHITE;

        Sprite() = default;

        Sprite(const Sprite &other) : texture(other.texture), id(other.id), offset(other.offset), tint(other.tint) {}

        Sprite &operator=(const Sprite &other) {
            if (other.texture) { texture.emplace(*other.texture); } else { texture = std::nullopt;};
            id = other.id;
            offset = other.offset;
            tint = other.tint;
            return *this;
        };

        void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity) {
            ImGui::Text("Sprite");
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
            auto width = std::abs(rect.width) * transform.scale.x;
            auto height = std::abs(rect.height) * transform.scale.y;
            auto size_vec = Vector2{width, height};
            DrawTexturePro(txt.texture, rect, Rectangle{transform.position.x, transform.position.y, width, height},
                           Vector2Multiply(sprite.offset, Vector2(width, height)), RAD2DEG * transform.rotation,
                           sprite.tint);
        }
    }
}
