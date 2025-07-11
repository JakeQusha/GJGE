#pragma once

#include <raylib.h>
#include <cstdint>
#define ImTxt(texture) (*reinterpret_cast<ImTextureID*>(&texture))
namespace ge {
struct MultiTexture {
    Texture2D texture;
    int32_t cell_size_x;
    int32_t cell_size_y;

    [[nodiscard]] auto rect() const -> Rectangle {
        return Rectangle{.x = 0.f,
                         .y = 0.f,
                         .width = static_cast<float>(texture.width),
                         .height = static_cast<float>(texture.height)};
    }

    [[nodiscard]] auto rect_multi(uint16_t sprite_id) const -> Rectangle {
        auto columns_in_texture = texture.width / cell_size_x;
        auto row = sprite_id / columns_in_texture;
        auto column = sprite_id % columns_in_texture;
        return Rectangle{.x = static_cast<float>(column * cell_size_x),
                         .y = static_cast<float>(row * cell_size_y),
                         .width = static_cast<float>(cell_size_x),
                         .height = static_cast<float>(cell_size_y)};
    }

    MultiTexture() = default;

    explicit MultiTexture(const Texture2D& texture2D)
        : texture(texture2D), cell_size_x(std::abs(texture.width)), cell_size_y(std::abs(texture.height)) {}

    MultiTexture(const Texture2D& texture2D, const uint16_t cell_size_x, const uint16_t cell_size_y)
        : texture(texture2D), cell_size_x(cell_size_x), cell_size_y(cell_size_y) {}
};

template <typename... Args>
    requires std::constructible_from<MultiTexture, Texture2D, Args...>
auto LoadMultiTexture(const char* fileName, Args&&... args) -> MultiTexture {
    const auto texture = LoadTexture(fileName);
    return MultiTexture(texture, args...);
}

} // namespace ge