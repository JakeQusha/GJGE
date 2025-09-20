#pragma once
#ifdef GJGE_DEV_TOOLS
#include <imgui.h>
#include "assets/asset_manager.hpp"
template <typename T>
void display_asset_manager_picker(const char* asset_name, std::optional<T>& asset, ge::AssetManager& am) {
    if (ImGui::BeginPopup(asset_name)) {
        for (auto&& [item_name, item] : am.get_all<T>()) {
            if (ImGui::MenuItem(item_name.c_str(), nullptr)) {
                asset = std::any_cast<T>(item);
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    if (ImGui::Button(asset_name)) {
        ImGui::OpenPopup(asset_name);
    }
}
#endif