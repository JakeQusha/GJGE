#pragma once

#include <any>
#include <concepts>
#include <format>
#include <fstream>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include "error.hpp"

namespace ge {
template <typename T>
concept Storable = std::default_initializable<T> && requires(T t, const T& ct, const std::string& str) {
    { ct.export_to_str() } -> std::same_as<std::string>;
    { t.import_from_str(str) } -> std::same_as<bool>;
};
class AssetManager {
public:
    using AssetMap = std::unordered_map<std::string, std::any>;

private:
    std::unordered_map<std::type_index, AssetMap> assets;

public:
    template <typename T>
    [[nodiscard]] auto get(const std::string& id) -> T& {
        auto* asset = get_ptr<T>(id);
        if (asset == nullptr) {
            fatal_error(std::format("Asset '{}' of type '{}' not found", id, typeid(T).name()));
        }
        return *asset;
    }

    template <typename T>
    [[nodiscard]] auto get_ptr(const std::string& id) -> T* {
        const auto type_it = assets.find(std::type_index(typeid(T)));
        if (type_it == assets.end()) {
            return nullptr;
        }
        const auto it = type_it->second.find(id);
        if (it == type_it->second.end()) {
            return nullptr;
        }
        return std::any_cast<T>(&it->second);
    }

    template <typename T>
    [[nodiscard]] auto get_all() -> AssetMap& {
        return assets[std::type_index(typeid(T))];
    }

    template <typename T>
    void add(const std::string& id, T&& asset) {
        assets[std::type_index(typeid(T))][id] = std::forward<T>(asset);
    }

    template <typename T, typename... Args>
    void emplace(const std::string& id, Args&&... args) {
        assets[std::type_index(typeid(T))].emplace(id, T(std::forward<Args>(args)...));
    }
    template <Storable T>
    void export_to_file(const std::string& path) {
        std::ofstream file(path);
        for (auto& [id, asset] : get_all<T>()) {
            file << id << ':';
            file << std::any_cast<T&>(asset).export_to_str() << '\n';
        }
    }
    template <Storable T>
    void import_from_file(const std::string& path) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            const auto id_end = line.find(':');
            if (id_end == std::string::npos) {
                continue;
            }
            auto id = line.substr(0, id_end);
            auto data = line.substr(id_end + 1);
            T asset;
            if (!asset.import_from_str(data)) {
                continue;
            }
            assets[std::type_index(typeid(T))][std::move(id)] = std::move(asset);
        }
    }
};
} // namespace ge
