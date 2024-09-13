#pragma once

#include <tuple>
#include <unordered_map>
#include <type_traits>
#include <imgui.h>
#include <any>

namespace ge {

class AssetManager {
    std::unordered_map<const char*, std::unordered_map<const char*, std::any>> assets;

public:
    template <typename T>
    [[nodiscard]] auto get(const char* id) -> T& {
        return std::any_cast<T&>(assets[typeid(T).name()].at(id));
    }

    template <typename T>
    [[nodiscard]] auto get_ptr(const char* id) -> T* {
        return std::any_cast<T*>(&assets[typeid(T).name()].at(id));
    }

    template <typename T>
    [[nodiscard]] auto get_all() -> std::unordered_map<const char*, std::any>& {
        return assets[typeid(T).name()];
    }

    template <typename T>
    void add(const char* id, T&& asset) {
        assets[typeid(T).name()][id] = std::forward<T>(asset);
    }

    template <typename T, typename... Args>
    void emplace(const char* id, Args&&... args) {
        assets[typeid(T).name()].emplace(id, T(std::forward<Args>(args)...));
    }
};
} // namespace ge