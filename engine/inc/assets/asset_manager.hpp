#pragma once

#include <tuple>
#include <unordered_map>
#include <type_traits>
#include <imgui.h>

namespace ge {
    template<typename... Types>
    class AssetManager {
        std::tuple<std::unordered_map<const char *, Types>...> assets;

        template<typename T, typename... Rest>
        struct are_unique : std::integral_constant<bool,
                (!std::is_same<T, Rest>::value && ...) && are_unique<Rest...>::value> {
        };
        template<typename T>
        struct are_unique<T> : std::true_type {
        };
        static_assert(are_unique<Types...>::value, "All types must be unique");

    public:
        template<typename T>
        [[nodiscard]]T &get(const char *id) {
            return std::get<std::unordered_map<const char *, T>>(assets).at(id);
        }

        template<typename T>
        [[nodiscard]]std::unordered_map<const char *, T> &get_map() {
            return std::get<std::unordered_map<const char *, T>>(assets);
        }

        template<typename T>
        void add(const char *id, T &&asset) {
            std::get<std::unordered_map<const char *, T>>(assets)[id] = std::forward<T>(asset);
        }

        template<typename T>
        void display_asset_picker(T* &result, const char *name) {
            if (ImGui::Button(name)) {
                ImGui::OpenPopup(name);
            }
            if (ImGui::BeginPopup(name)) {
                for(auto &[asset_name, asset] : std::get<std::unordered_map<const char *, T>>(assets)){
                    if (ImGui::MenuItem(asset_name, nullptr)) {
                        result = &asset;
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndPopup();
            }
        }
    };
}