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

    void assign_key(KeyboardKey key, const std::string& id);

    [[nodiscard]] auto get_key(const std::string& id) const -> KeyboardKey;

private:
    struct AssignedKey {
        KeyboardKey key;
        std::vector<subscriber_id_t> subscribers;
    };
    std::unordered_map<std::string, AssignedKey> keys;
    struct Subscriber {
        KeyboardEvent type;
        callback_t callback;
        subscriber_id_t id;
    };

    [[nodiscard]] static auto make_subscriber(callback_t&& callback, KeyboardEvent type) -> Subscriber;

public:
    auto subscribe(KeyboardEvent type, KeyboardKey key, callback_t&& callback) -> subscriber_id_t;

    auto subscribe(KeyboardEvent type, const std::string& key, callback_t&& callback) -> subscriber_id_t;

    void unsubscribe(subscriber_id_t id);

    void wipe(bool wipe_binds = false);

    std::unordered_map<KeyboardKey, std::vector<Subscriber>> subscribers;
};

void notify_keyboard_press_system(const KeyManager& manager);

} // namespace ge
