#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <entt.hpp>
namespace ge {
enum class InputEvent : uint8_t { PRESS, RELEASE, UP, DOWN };
enum class InputType : uint8_t { KEYBOARD, MOUSE };
struct Input {
    InputType type;
    union {
        KeyboardKey key;
        MouseButton button;
    };
    static auto keyboard(KeyboardKey key) -> Input;
    static auto mouse(MouseButton button) -> Input;
    bool operator==(const Input& i) const;
    bool operator<(const Input& i) const;
};


class KeyManager {
public:
    using callback_t = std::function<void()>;
    using subscriber_id_t = std::uint32_t;

    void assign_key(Input input, const std::string& id);

    [[nodiscard]] auto get_key(const std::string& id) const -> Input;

private:
    struct AssignedKey {
        Input input;
        std::vector<subscriber_id_t> subscribers;
    };
    std::unordered_map<std::string, AssignedKey> keys;
    std::unordered_map<std::string, std::set<subscriber_id_t>> groups;
    std::unordered_map<subscriber_id_t, std::string> id_to_group;
    struct Subscriber {
        InputEvent type;
        callback_t callback;
        subscriber_id_t id;
    };

    [[nodiscard]] static auto make_subscriber(callback_t&& callback, InputEvent type) -> Subscriber;

public:
    auto subscribe(InputEvent type, const std::string& key, callback_t&& callback) -> subscriber_id_t;
    auto subscribe(InputEvent type, const std::string& key, const std::string& group, callback_t&& callback) -> subscriber_id_t;
    void unsubscribe(subscriber_id_t id);

    void wipe(bool wipe_binds = false);
    void wipe_group(const std::string& group);
    void disable_group(const std::string& group);
    void enable_group(const std::string& group);
    [[nodiscard]] auto is_disabled(subscriber_id_t id) const -> bool;
    std::map<Input, std::vector<Subscriber>> subscribers{};
    std::set<std::string> disabled_groups;
};

void notify_keyboard_press_system(const KeyManager& manager);

} // namespace ge