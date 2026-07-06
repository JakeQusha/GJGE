#include "assets/animation.hpp"
#include <charconv>
#include <sstream>

namespace {
template <typename T>
auto parse_number(const std::string& token, T& out) -> bool {
    const auto* begin = token.data();
    const auto* end = token.data() + token.size();
    const auto [ptr, ec] = std::from_chars(begin, end, out);
    return ec == std::errc{} && ptr == end;
}
} // namespace

auto ge::Animation::import_from_str(const std::string& input) -> bool {
    std::stringstream ss(input);
    std::string token;
    if (!std::getline(ss, token, ';')) {
        return false;
    }
    texture = token;
    if (!std::getline(ss, token, ';') || !parse_number(token, frames)) {
        return false;
    }
    if (!std::getline(ss, token, ';') || !parse_number(token, sample_rate)) {
        return false;
    }
    if (!std::getline(ss, token, ';') || !parse_number(token, start_frame)) {
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
