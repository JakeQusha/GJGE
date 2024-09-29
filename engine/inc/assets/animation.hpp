#pragma once
#include <cstdint>
#include <string>
#include "assets/asset_manager.hpp"
namespace ge {
struct Animation {
    std::string texture;
    uint16_t frames;
    uint8_t sample_rate;
    uint16_t start_frame;
    auto import_from_str(const std::string& input) -> bool;
    [[nodiscard]] auto export_to_str() const -> std::string;
};
} // namespace ge
