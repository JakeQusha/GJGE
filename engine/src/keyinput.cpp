#include <algorithm>
#include <raylib.h>
#include "keyinput.hpp"
#include <ranges>

[[nodiscard]] auto ge::KeyManager::make_subscriber(callback_t&& callback, const InputEvent type) -> Subscriber {
    static subscriber_id_t id = 0;
    return Subscriber{.type = type, .callback = std::move(callback), .id = id++};
}

// auto ge::KeyManager::subscribe(const KeyboardEvent type, const KeyboardKey key, callback_t&& callback) -> subscriber_id_t {
//     auto subscriber = make_subscriber(std::move(callback), type);
//     const auto id = subscriber.id;
//     subscribers[key].push_back(std::move(subscriber));
//     return id;
// }
auto ge::KeyManager::subscribe(const InputEvent type, const std::string& key, const std::string& group, callback_t&& callback) -> subscriber_id_t {
    const auto id = subscribe(type, key, std::forward<decltype(callback)>(callback));
    groups[group].insert(id);
    id_to_group[id] = group;
    return id;
}

void ge::KeyManager::unsubscribe(subscriber_id_t id) {
    for (auto& subs : subscribers | std::views::values) {
        std::erase_if(subs, [id](const auto& sub) { return sub.id == id; });
    }
    if (id_to_group.contains(id)) {
        groups.at(id_to_group.at(id)).erase(id);
        id_to_group.erase(id);
    }
}
void ge::KeyManager::wipe(const bool wipe_binds) {
    if (wipe_binds) {
        keys.clear();
    }
    for (auto& subs : subscribers | std::views::values) {
        subs.clear();
    }
    groups.clear();
    id_to_group.clear();
    disabled_groups.clear();
}
void ge::KeyManager::wipe_group(const std::string& group) {
    for (auto id : groups.at(group)) {
        for (auto& subs : subscribers | std::views::values) {
            std::erase_if(subs, [id](const auto& sub) { return sub.id == id; });
        }
        id_to_group.erase(id);
    }
    groups.at(group).clear();
}

auto ge::KeyManager::subscribe(const InputEvent type, const std::string& key, callback_t&& callback) -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    auto& ak = keys.at(key);
    subscribers[ak.input].push_back(std::move(subscriber));
    ak.subscribers.push_back(id);
    return id;
}

auto ge::KeyManager::get_key(const std::string& id) const -> Input { return keys.at(id).input; }

auto ge::Input::keyboard(const KeyboardKey key) -> Input { return Input{.type = InputType::KEYBOARD, .key = key}; }
auto ge::Input::mouse(const MouseButton button) -> Input { return Input{.type = InputType::MOUSE, .button = button}; }
bool ge::Input::operator==(const Input& i) const {
    if (type != i.type) {
        return false;
    }
    switch (type) {
    case InputType::KEYBOARD:
        return key == i.key;
    case InputType::MOUSE:
        return button == i.button;
    default:
        return false;
    }
}
bool ge::Input::operator<(const Input& i) const {
    if (type != i.type) {
        return type < i.type;
    }
    switch (type) {
    case InputType::KEYBOARD:
        return key < i.key;
    case InputType::MOUSE:
        return button < i.button;
    default:
        return false;
    }
}
void ge::KeyManager::assign_key(const Input input, const std::string& id) {
    // check if key is already assigned
    if (!keys.contains(id)) {
        keys[id].input = input;
        return;
    }
    const auto old_key = keys.at(id).input;
    keys[id].input = input;
    for (const auto sub : keys.at(id).subscribers) {
        for (size_t i = 0; i < subscribers[old_key].size(); ++i) {
            if (subscribers[old_key][i].id == sub) {
                auto callback = std::move(subscribers[old_key][i].callback);
                const auto event = subscribers[old_key][i].type;
                subscribers[old_key].erase(subscribers[old_key].begin() + static_cast<long>(i));
                auto subscriber = Subscriber{.type = event, .callback = std::move(callback), .id = sub};
                subscribers[input].push_back(std::move(subscriber));
                break;
            }
        }
    }
}
void ge::KeyManager::disable_group(const std::string& group) { disabled_groups.insert(group); }
void ge::KeyManager::enable_group(const std::string& group) { disabled_groups.erase(group); }
[[nodiscard]] auto ge::KeyManager::is_disabled(const subscriber_id_t id) const -> bool {
    return id_to_group.contains(id) && disabled_groups.contains(id_to_group.at(id));
}

void ge::notify_keyboard_press_system(const KeyManager& manager) {
    for (const auto& [fst, snd] : manager.subscribers) {
        for (const auto& sub : snd) {
            if (!manager.disabled_groups.empty() && manager.is_disabled(sub.id)) {
                continue;
            }
            using ST = InputEvent;
            switch (fst.type) {
            case InputType::KEYBOARD:
                if ((sub.type == ST::PRESS && IsKeyPressed(fst.key)) || (sub.type == ST::RELEASE && IsKeyReleased(fst.key)) ||
                    (sub.type == ST::UP && IsKeyUp(fst.key)) || (sub.type == ST::DOWN && IsKeyDown(fst.key))) {
                    sub.callback();
                }
                break;
            case InputType::MOUSE:
                if ((sub.type == ST::PRESS && IsMouseButtonPressed(fst.button)) || (sub.type == ST::RELEASE && IsMouseButtonReleased(fst.button)) ||
                    (sub.type == ST::UP && IsMouseButtonUp(fst.button)) || (sub.type == ST::DOWN && IsMouseButtonDown(fst.button))) {
                    sub.callback();
                }
                break;
            }
        }
    }
}
