#pragma once

#include <rl.hpp>
#include <imgui.h>
#include <algorithm>
#include <any>
#include <cstdint>
#include <format>
#include "error.hpp"
#include "asset_manager.hpp"

namespace ge {
[[nodiscard]] inline auto ImTxt(const Texture2D& texture) -> ImTextureID { return static_cast<ImTextureID>(texture.id); }


struct MultiTexture {
    Texture2D texture{};
    int32_t cell_size_x{1};
    int32_t cell_size_y{1};

    [[nodiscard]] auto rect() const -> Rectangle {
        return Rectangle{.x = 0.f,
                         .y = 0.f,
                         .width = static_cast<float>(texture.width),
                         .height = static_cast<float>(texture.height)};
    }

    [[nodiscard]] auto rect_multi(uint16_t sprite_id) const -> Rectangle {
        const auto columns_in_texture = std::max(texture.width / cell_size_x, 1);
        const auto row = sprite_id / columns_in_texture;
        const auto column = sprite_id % columns_in_texture;
        return Rectangle{.x = static_cast<float>(column * cell_size_x),
                         .y = static_cast<float>(row * cell_size_y),
                         .width = static_cast<float>(cell_size_x),
                         .height = static_cast<float>(cell_size_y)};
    }

    MultiTexture() = default;

    explicit MultiTexture(const Texture2D& texture2D)
        : texture(texture2D), cell_size_x(std::max(std::abs(texture.width), 1)),
          cell_size_y(std::max(std::abs(texture.height), 1)) {}

    MultiTexture(const Texture2D& texture2D, const uint16_t cell_size_x, const uint16_t cell_size_y)
        : texture(texture2D), cell_size_x(std::max<int32_t>(cell_size_x, 1)),
          cell_size_y(std::max<int32_t>(cell_size_y, 1)) {}
};

template <typename... Args>
    requires std::constructible_from<MultiTexture, Texture2D, Args...>
auto LoadMultiTexture(const char* fileName, Args&&... args) -> MultiTexture {
    const auto texture = LoadTexture(fileName);
    if (texture.id == 0) {
        fatal_error(std::format("Failed to load texture: {}", fileName));
    }
    return MultiTexture(texture, std::forward<Args>(args)...);
}


inline void UnloadAllMultiTextures(AssetManager& asset_manager) {
    for (auto& [id, asset] : asset_manager.get_all<MultiTexture>()) {
        UnloadTexture(std::any_cast<MultiTexture&>(asset).texture);
    }
}

} // namespace ge
