#pragma once

#include <functional>
#include <vector>
#include <cstdint>
#include <entt.hpp>

namespace ge {
enum class KeyboardEvent : uint8_t { PRESS, RELEASE, UP, DOWN };

class KeyManager {
public:
    using callback_t = std::function<void()>;
    using subscriber_id_t = std::uint32_t;

    void assign_key(KeyboardKey key, const char* id);

    [[nodiscard]] auto get_key(const char* id) -> KeyboardKey;

private:
    struct AssignedKey {
        KeyboardKey key;
        std::vector<subscriber_id_t> subscribers;
    };
    std::unordered_map<const char*, AssignedKey> keys;
    struct Subscriber {
        KeyboardEvent type;
        callback_t callback;
        subscriber_id_t id;
    };

    [[nodiscard]] static auto make_subscriber(callback_t&& callback, KeyboardEvent type) -> Subscriber;

public:
    auto subscribe(KeyboardEvent type, KeyboardKey key, callback_t&& callback) -> subscriber_id_t;

    auto subscribe(KeyboardEvent type, const char* key, callback_t&& callback) -> subscriber_id_t;

    void unsubscribe(subscriber_id_t id);

    std::unordered_map<KeyboardKey, std::vector<Subscriber>> subscribers;
};

void notify_keyboard_press_system(KeyManager& manager);

} // namespace ge
