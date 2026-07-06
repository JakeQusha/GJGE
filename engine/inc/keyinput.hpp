#pragma once
#include <raylib.h>
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
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
    auto operator==(const Input& i) const -> bool;
    auto operator<(const Input& i) const -> bool;
};

class KeyManager {
public:
    using callback_t = std::function<void()>;
    using subscriber_id_t = std::uint32_t;

    void assign_key(Input input, const std::string& id);

    [[nodiscard]] auto get_key(const std::string& id) const -> Input;

    auto subscribe(InputEvent type, const std::string& key, callback_t&& callback) -> subscriber_id_t;
    auto subscribe(InputEvent type, const std::string& key, const std::string& group, callback_t&& callback) -> subscriber_id_t;
    void unsubscribe(subscriber_id_t id);

    void wipe(bool wipe_binds = false);
    void wipe_group(const std::string& group);
    void disable_group(const std::string& group);
    void enable_group(const std::string& group);
    [[nodiscard]] auto is_disabled(subscriber_id_t id) const -> bool;

private:
    struct AssignedKey {
        Input input;
        std::vector<subscriber_id_t> subscribers;
    };
    struct Subscriber {
        InputEvent type;
        callback_t callback;
        subscriber_id_t id;
    };

    [[nodiscard]] auto make_subscriber(callback_t&& callback, InputEvent type) -> Subscriber;

    subscriber_id_t next_id = 0;
    std::unordered_map<std::string, AssignedKey> keys;
    std::unordered_map<std::string, std::set<subscriber_id_t>> groups;
    std::unordered_map<subscriber_id_t, std::string> id_to_group;
    std::map<Input, std::vector<Subscriber>> subscribers;
    std::set<std::string> disabled_groups;

    friend void notify_keyboard_press_system(const KeyManager& manager);
};

void notify_keyboard_press_system(const KeyManager& manager);

} // namespace ge
