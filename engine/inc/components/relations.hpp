#pragma once
#include <entt.hpp>
#include <imgui.h>
namespace ge::comp {
    struct Parent {
        static constexpr auto name = "Parent";
        std::vector<entt::entity> children;
        void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity){
            ImGui::Text("Children:");
            for (auto child : children){
                ImGui::Text("%d",static_cast<uint32_t>(child));
            }
        }
    };
    struct Child {
        static constexpr auto name = "Child";
        entt::entity parent;
        void inspect([[maybe_unused]] entt::registry &registry, [[maybe_unused]] entt::entity entity){
            ImGui::Text("%s",std::format("Parent: {}",static_cast<uint32_t>(parent)).c_str());
        }
    };

bool add_relation(entt::registry& registry,entt::entity parent,entt::entity child);
bool remove_relation(entt::registry& registry,entt::entity parent,entt::entity child);
void relations_call_on_delete_entity(entt::registry& registry, entt::entity entity);
}
