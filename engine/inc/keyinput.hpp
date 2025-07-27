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
    std::unordered_map<std::string, std::set<subscriber_id_t>> groups;
    std::unordered_map<subscriber_id_t, std::string> id_to_group;
    struct Subscriber {
        KeyboardEvent type;
        callback_t callback;
        subscriber_id_t id;
    };

    [[nodiscard]] static auto make_subscriber(callback_t&& callback, KeyboardEvent type) -> Subscriber;

public:
    [[deprecated]] auto subscribe(KeyboardEvent type, KeyboardKey key, callback_t&& callback) -> subscriber_id_t;

    auto subscribe(KeyboardEvent type, const std::string& key, callback_t&& callback) -> subscriber_id_t;
    auto subscribe(KeyboardEvent type, const std::string& key, const std::string& group, callback_t&& callback) -> subscriber_id_t;
    void unsubscribe(subscriber_id_t id);

    void wipe(bool wipe_binds = false);
    void wipe_group(const std::string& group);
    void disable_group(const std::string& group);
    void enable_group(const std::string& group);
    [[nodiscard]] auto is_disabled(subscriber_id_t id) const -> bool;
    std::unordered_map<KeyboardKey, std::vector<Subscriber>> subscribers;
    std::set<std::string> disabled_groups;
};

void notify_keyboard_press_system(const KeyManager& manager);

} // namespace ge
