#include "components/transform2D.hpp"
void ge::comp::Transform2D::inspect(entt::registry &registry, entt::entity entity) {
    ImGui::DragFloat2("Position", &position.x, 1.f);
    ImGui::DragFloat("Rotation", &rotation, 0.02f);
    ImGui::DragFloat2("Scale", &scale.x, 0.02f);
}
