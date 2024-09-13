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
    [[nodiscard]] T& get(const char* id) {
        return std::any_cast<T&>(assets[typeid(T).name()].at(id));
    }

    template <typename T>
    [[nodiscard]] T* get_ptr(const char* id) {
        return std::any_cast<T*>(&assets[typeid(T).name()].at(id));
    }

    template <typename T>
    [[nodiscard]] std::unordered_map<const char*, std::any>& get_all() {
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