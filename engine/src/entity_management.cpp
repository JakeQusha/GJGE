#include "entity_management.hpp"
#include "components/transform2D.hpp"
#include "gui/inspector.hpp"

static std::queue<entt::entity> kill_queue;

void ge::kill_unsafe(entt::registry& registry, entt::entity entity) {
    if (!registry.valid(entity)) {
        return;
    }
    relations_call_on_delete_entity(registry, entity);
    registry.destroy(entity);
}
void ge::empty_kill_queue(entt::registry& registry) {
    while (!kill_queue.empty()) {
        kill_unsafe(registry,kill_queue.front());
        kill_queue.pop();
    }
}
void ge::kill(const entt::entity entity) {
    kill_queue.push(entity);
}
auto ge::create(entt::registry& registry, const char* name) -> entt::entity {
    const auto entity = registry.create();
    registry.emplace<InspectorIntegration>(entity, name);
    registry.emplace<comp::Transform2D>(entity);
    return entity;
}
auto ge::create_child(entt::registry& registry, entt::entity parent, const char* name) -> entt::entity {
    const auto entity = create(registry, name);
    add_relation(registry, parent, entity);
    return entity;
}
