#include <algorithm>
#include <rl.hpp>
#include "keyinput.hpp"
#include "error.hpp"
#include <format>
#include <ranges>
#include <vector>

[[nodiscard]] auto ge::KeyManager::make_subscriber(callback_t&& callback, const InputEvent type) -> Subscriber {
    return Subscriber{.type = type, .callback = std::move(callback), .id = next_id++};
}

auto ge::KeyManager::subscribe(const InputEvent type, const std::string& key, const std::string& group, callback_t&& callback) -> subscriber_id_t {
    const auto id = subscribe(type, key, std::move(callback));
    groups[group].insert(id);
    id_to_group[id] = group;
    return id;
}

void ge::KeyManager::unsubscribe(subscriber_id_t id) {
    for (auto& subs : subscribers | std::views::values) {
        std::erase_if(subs, [id](const auto& sub) { return sub.id == id; });
    }
    for (auto& ak : keys | std::views::values) {
        std::erase(ak.subscribers, id);
    }
    if (id_to_group.contains(id)) {
        groups.at(id_to_group.at(id)).erase(id);
        id_to_group.erase(id);
    }
}
void ge::KeyManager::wipe(const bool wipe_binds) {
    if (wipe_binds) {
        keys.clear();
    } else {
        for (auto& ak : keys | std::views::values) {
            ak.subscribers.clear();
        }
    }
    subscribers.clear();
    groups.clear();
    id_to_group.clear();
    disabled_groups.clear();
}
void ge::KeyManager::wipe_group(const std::string& group) {
    const auto group_it = groups.find(group);
    if (group_it == groups.end()) {
        return;
    }
    for (auto id : group_it->second) {
        for (auto& subs : subscribers | std::views::values) {
            std::erase_if(subs, [id](const auto& sub) { return sub.id == id; });
        }
        for (auto& ak : keys | std::views::values) {
            std::erase(ak.subscribers, id);
        }
        id_to_group.erase(id);
    }
    group_it->second.clear();
}

auto ge::KeyManager::subscribe(const InputEvent type, const std::string& key, callback_t&& callback) -> subscriber_id_t {
    const auto key_it = keys.find(key);
    if (key_it == keys.end()) {
        fatal_error(std::format("KeyManager: no input assigned for '{}', call assign_key first", key));
    }
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    auto& ak = key_it->second;
    subscribers[ak.input].push_back(std::move(subscriber));
    ak.subscribers.push_back(id);
    return id;
}

auto ge::KeyManager::get_key(const std::string& id) const -> Input {
    const auto it = keys.find(id);
    if (it == keys.end()) {
        fatal_error(std::format("KeyManager: no input assigned for '{}'", id));
    }
    return it->second.input;
}

auto ge::Input::keyboard(const KeyboardKey key) -> Input { return Input{.type = InputType::KEYBOARD, .key = key}; }
auto ge::Input::mouse(const MouseButton button) -> Input { return Input{.type = InputType::MOUSE, .button = button}; }
auto ge::Input::operator==(const Input& i) const -> bool {
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
auto ge::Input::operator<(const Input& i) const -> bool {
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
    std::vector<KeyManager::callback_t> to_fire;
    for (const auto& [input, subs] : manager.subscribers) {
        for (const auto& sub : subs) {
            if (!manager.disabled_groups.empty() && manager.is_disabled(sub.id)) {
                continue;
            }
            using ST = InputEvent;
            bool fire = false;
            switch (input.type) {
            case InputType::KEYBOARD:
                fire = (sub.type == ST::PRESS && rl::Keyboard::IsKeyPressed(input.key)) || (sub.type == ST::RELEASE && rl::Keyboard::IsKeyReleased(input.key)) ||
                       (sub.type == ST::UP && rl::Keyboard::IsKeyUp(input.key)) || (sub.type == ST::DOWN && rl::Keyboard::IsKeyDown(input.key));
                break;
            case InputType::MOUSE:
                fire = (sub.type == ST::PRESS && rl::Mouse::IsButtonPressed(input.button)) ||
                       (sub.type == ST::RELEASE && rl::Mouse::IsButtonReleased(input.button)) ||
                       (sub.type == ST::UP && rl::Mouse::IsButtonUp(input.button)) || (sub.type == ST::DOWN && rl::Mouse::IsButtonDown(input.button));
                break;
            }
            if (fire) {
                to_fire.push_back(sub.callback);
            }
        }
    }
    for (auto& callback : to_fire) {
        callback();
    }
}
