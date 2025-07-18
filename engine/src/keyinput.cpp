#include <algorithm>
#include <raylib.h>
#include "keyinput.hpp"
#include <ranges>

[[nodiscard]] auto ge::KeyManager::make_subscriber(callback_t&& callback, const KeyboardEvent type) -> Subscriber {
    static subscriber_id_t id = 0;
    return Subscriber{.type = type, .callback = std::move(callback), .id = id++};
}

auto ge::KeyManager::subscribe(KeyboardEvent type, KeyboardKey key, callback_t&& callback) -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    subscribers[key].push_back(std::move(subscriber));
    return id;
}
auto ge::KeyManager::subscribe(ge::KeyboardEvent type, const std::string& key, const std::string& group,
                               ge::KeyManager::callback_t&& callback) -> ge::KeyManager::subscriber_id_t {
    auto id = subscribe(type, key, std::forward<decltype(callback)>(callback));
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

auto ge::KeyManager::subscribe(const KeyboardEvent type, const std::string& key, callback_t&& callback)
    -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    auto& ak = keys.at(key);
    subscribers[ak.key].push_back(std::move(subscriber));
    ak.subscribers.push_back(id);
    return id;
}

auto ge::KeyManager::get_key(const std::string& id) const -> KeyboardKey { return keys.at(id).key; }

void ge::KeyManager::assign_key(const KeyboardKey key, const std::string& id) {
    // check if key is already assigned
    if (!keys.contains(id)) {
        keys[id].key = key;
        return;
    }
    const auto old_key = keys.at(id).key;
    keys[id].key = key;
    for (auto& sub : keys.at(id).subscribers) {
        for (size_t i = 0; i < subscribers[old_key].size(); ++i) {
            if (subscribers[old_key][i].id == sub) {
                auto callback = std::move(subscribers[old_key][i].callback);
                const auto event = subscribers[old_key][i].type;
                subscribers[old_key].erase(subscribers[old_key].begin() + static_cast<long>(i));
                auto subscriber = Subscriber{.type = event, .callback = std::move(callback), .id = sub};
                subscribers[key].push_back(std::move(subscriber));
                break;
            }
        }
    }
}
void ge::KeyManager::disable_group(const std::string& group) { disabled_groups.insert(group); }
void ge::KeyManager::enable_group(const std::string& group) { disabled_groups.erase(group); }
[[nodiscard]] bool ge::KeyManager::is_disabled(const ge::KeyManager::subscriber_id_t id) const {
    return disabled_groups.contains(id_to_group.at(id));
}

void ge::notify_keyboard_press_system(const KeyManager& manager) {
    for (const auto& [fst, snd] : manager.subscribers) {
        for (const auto& sub : snd) {
            if (!manager.disabled_groups.empty() && manager.is_disabled(sub.id)) {
                continue;
            }
            using ST = KeyboardEvent;
            if ((sub.type == ST::PRESS && IsKeyPressed(fst)) || (sub.type == ST::RELEASE && IsKeyReleased(fst)) ||
                (sub.type == ST::UP && IsKeyUp(fst)) || (sub.type == ST::DOWN && IsKeyDown(fst))) {
                sub.callback();
            }
        }
    }
}
