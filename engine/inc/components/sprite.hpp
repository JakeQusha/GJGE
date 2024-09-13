#pragma once

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <optional>
#include <entt.hpp>
#include "assets/texture.hpp"
#include "transform2D.hpp"
#include "gui/inspector_comp.hpp"
#include "entity_management.hpp"

namespace ge {
namespace comp {
struct Sprite {
    static constexpr Dependencies<Transform2D> dependencies{};
    static constexpr auto name = "Sprite";
    std::optional<MultiTexture> texture = std::nullopt;
    uint16_t id = 0;
    Vector2 offset{0.5f, 0.5f};
    Color tint = WHITE;

    void inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity) {
        auto am = registry.ctx().get<ge::AssetManager>();
        display_asset_manager_picker<ge::MultiTexture>("Texture", texture, am);
        ImGui::DragFloat2("Offset", &offset.x, 0.01f);
        ImGui::DragScalar("Sprite id", ImGuiDataType_U16, &id, 1.0f);
    }
};
} // namespace comp

void draw_sprites(entt::registry& registry);

} // namespace ge
