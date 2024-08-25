#include <algorithm>
#include <raylib.h>
#include "keyinput.hpp"

[[nodiscard]] auto ge::KeyManager::make_subscriber(callback_t &&callback, KeyboardEvent type) -> Subscriber {
    static subscriber_id_t id = 0;
    return Subscriber{type, std::move(callback), id++};
}

auto ge::KeyManager::subscribe(KeyboardEvent type, KeyboardKey key, callback_t &&callback) -> subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    subscribers[key].push_back(std::move(subscriber));
    return id;
}

void ge::KeyManager::unsubscribe(subscriber_id_t id) {
    for (auto &[key, subs]: subscribers) {
        subs.erase(std::remove_if(subs.begin(), subs.end(), [id](const auto &sub) { return sub.id == id; }),
                   subs.end());
    }
}

auto ge::KeyManager::subscribe(KeyboardEvent type, const char *key,
                               callback_t &&callback) -> ge::KeyManager::subscriber_id_t {
    auto subscriber = make_subscriber(std::move(callback), type);
    const auto id = subscriber.id;
    auto &ak = keys.at(key);
    subscribers[ak.key].push_back(std::move(subscriber));
    ak.subscribers.push_back(id);
    return id;
}

auto ge::KeyManager::get_key(const char *id) -> KeyboardKey {
    return keys.at(id).key;
}

void ge::KeyManager::assign_key(KeyboardKey key, const char *id) {
    //check if key is already assigned
    if (!keys.contains(id)) {
        keys[id].key = key;
        return;
    }
    const auto old_key = keys.at(id).key;
    for (auto &sub: keys.at(id).subscribers) {
        for (size_t i = 0; i < subscribers[old_key].size(); ++i) {
            if (subscribers[old_key][i].id == sub) {
                auto callback = std::move(subscribers[old_key][i].callback);
                auto event = subscribers[old_key][i].type;
                subscribers[old_key].erase(subscribers[old_key].begin() + i);
                auto subscriber = make_subscriber(std::move(callback), event);
                subscribers[key].push_back(std::move(subscriber));
                sub = subscriber.id;
                break;
            }
        }
    }

}

void ge::notify_keyboard_press_system(ge::KeyManager &manager) {
    for (const auto &key_val: manager.subscribers) {
        for (const auto &sub: key_val.second) {
            using ST = KeyboardEvent;
            if ((sub.type == ST::PRESS && IsKeyPressed(key_val.first)) ||
                (sub.type == ST::RELEASE && IsKeyReleased(key_val.first)) ||
                (sub.type == ST::UP && IsKeyUp(key_val.first)) ||
                (sub.type == ST::DOWN && IsKeyDown(key_val.first))) {
                sub.callback();
            }
        }
    }
}
