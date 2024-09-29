#include "assets/animation.hpp"
#include <sstream>
auto ge::Animation::import_from_str(const std::string& input) -> bool {
    std::stringstream ss(input);
    std::string token;
    if (!std::getline(ss, token, ';')) {
        return false;
    }
    texture = token;
    if (!std::getline(ss, token, ';')) {
        return false;
    }
    try {
        frames = static_cast<uint16_t>(std::stoi(token));
    } catch (std::invalid_argument& e) {
        return false;
    }
    if (!std::getline(ss, token, ';')) {
        return false;
    }
    try {
        sample_rate = static_cast<uint8_t>(std::stoi(token));
    } catch (std::invalid_argument& e) {
        return false;
    }
    if (!std::getline(ss, token, ';')) {
        return false;
    }
    try {
        start_frame = static_cast<uint16_t>(std::stoi(token));
    } catch (std::invalid_argument& e) {
        return false;
    }
    return true;
}
auto ge::Animation::export_to_str() const -> std::string {
    std::string output;
    output += texture + ';';
    output += std::to_string(frames) + ';';
    output += std::to_string(sample_rate) + ';';
    output += std::to_string(start_frame) + ';';
    return output;
}
