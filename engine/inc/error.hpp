#pragma once
#include <cstdlib>
#include <expected>
#include <print>
#include <string>

namespace ge {
using Error = std::string;
template <typename T>
using Expected = std::expected<T, Error>;

[[noreturn]] inline void fatal_error(const Error& error) {
    std::println(stderr, "Error: {}", error);
    std::exit(1);
}

template <typename T>
auto unwrap(const Expected<T>& exp) -> T {
    if (!exp.has_value()) {
        fatal_error(exp.error());
    }

    return exp.value();
}

} // namespace ge
