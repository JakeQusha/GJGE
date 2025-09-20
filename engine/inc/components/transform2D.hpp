#pragma once

#include <raylib.h>
#include <entt.hpp>

namespace ge {
namespace comp {
struct Transform2D {
    static constexpr auto name = "Transform2D";
    Vector2 global_position{0, 0};
    float global_rotation{.0f};
    Vector2 global_scale{1, 1};
    Vector2 position{0, 0};
    float rotation{.0f};
    Vector2 scale{1, 1};
    Vector2 _last_position{0, 0};
    float _last_rotation{.0f};
    Vector2 _last_scale{1, 1};
#ifdef GJGE_DEV_TOOLS
    void inspect([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity);
#endif
};
} // namespace comp
void calculate_global_transform(entt::registry& registry);

} // namespace ge