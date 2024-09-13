#include "error.hpp"
#include "assets/assets_loader.hpp"
#include <expected>
#include <filesystem>

namespace ge {

[[nodiscard]] auto get_asset_path(const std::filesystem::path& resource_path) -> Expected<std::filesystem::path> {
    // If we have a resources directory defined, use that
#ifdef RESOURCES_DIR
    auto full_path = std::filesystem::path(RESOURCES_DIR) / resource_path;
    if (std::filesystem::exists(full_path)) {
        return full_path;
    }
#endif

    // Otherwise, try the path provided in the `resource_path` variable
    if (std::filesystem::exists(resource_path)) {
        return resource_path;
    }

    return std::unexpected(std::string{"Resource not found: "} + resource_path.string());
}

} // namespace ge
