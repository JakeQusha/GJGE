#pragma once
#include <concepts>
#include <filesystem>
#include <type_traits>
#include <utility>
#include "error.hpp"

namespace ge {
[[nodiscard]] auto get_asset_path(const std::filesystem::path& resource_path) -> Expected<std::filesystem::path>;

template <typename Func, typename... Args>
    requires((std::convertible_to<Args, std::filesystem::path> && ...) &&
             std::invocable<Func, std::conditional_t<true, const char*, Args>...>)
[[nodiscard]] auto load_asset(Func func, Args&&... args) {
    return func(unwrap(get_asset_path(std::forward<Args>(args))).string().c_str()...);
}

} // namespace ge
