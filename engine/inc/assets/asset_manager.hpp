#pragma once

#include <tuple>
#include <unordered_map>
#include <any>
#include <fstream>

namespace ge {
template <typename T>
concept Storable = requires(T t, const std::string& str) {
    { t.export_to_str() } -> std::same_as<std::string>;
    { T::import_from_str(str) } -> std::same_as<bool>;
};
class AssetManager {
    std::unordered_map<const char*, std::unordered_map<std::string, std::any>> assets;

public:
    template <typename T>
    [[nodiscard]] auto get(const std::string& id) -> T& {
        return std::any_cast<T&>(assets[typeid(T).name()].at(id));
    }

    template <typename T>
    [[nodiscard]] auto get_ptr(const std::string& id) -> T* {
        return &std::any_cast<T&>(assets[typeid(T).name()].at(id));
    }

    template <typename T>
    [[nodiscard]] auto get_all() -> std::unordered_map<std::string, std::any>& {
        return assets[typeid(T).name()];
    }

    template <typename T>
    void add(const std::string& id, T&& asset) {
        assets[typeid(T).name()][id] = std::forward<T>(asset);
    }

    template <typename T, typename... Args>
    void emplace(const std::string& id, Args&&... args) {
        assets[typeid(T).name()].emplace(id, T(std::forward<Args>(args)...));
    }
    template <Storable T>
    void export_to_file(const std::string& path) {
        std::ofstream file(path);
        for (auto& [id, asset] : assets[typeid(T).name()]) {
            file << id << ':';
            file << std::any_cast<T&>(asset).export_to_str() << '\n';
        }
    }
    template <Storable T>
    void import_from_file(const std::string& path) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            auto id_end = line.find(':');
            auto id = line.substr(0, id_end);
            auto data = line.substr(id_end + 1);
            T asset;
            if (!asset.import_from_str(data)) {
                continue;
            }
            assets[typeid(T).name()][id] = asset;
        }
    }
};
} // namespace ge