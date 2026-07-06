#include "entity_management.hpp"
#include "components/transform2D.hpp"
#include "components/relations.hpp"
#include "gui/inspector.hpp"
#include <vector>

namespace {
struct KillQueue {
    std::vector<entt::entity> entities;
};
} // namespace

void ge::kill_unsafe(entt::registry& registry, entt::entity entity) {
    if (!registry.valid(entity)) {
        return;
    }
    relations_call_on_delete_entity(registry, entity);
    registry.destroy(entity);
}
void ge::empty_kill_queue(entt::registry& registry) {
    auto* queue = registry.ctx().find<KillQueue>();
    if (queue == nullptr) {
        return;
    }
    for (size_t i = 0; i < queue->entities.size(); ++i) {
        kill_unsafe(registry, queue->entities[i]);
    }
    queue->entities.clear();
}
void ge::kill(entt::registry& registry, const entt::entity entity) {
    registry.ctx().emplace<KillQueue>().entities.push_back(entity);
}
auto ge::create(entt::registry& registry, const char* name) -> entt::entity {
    const auto entity = registry.create();
#ifdef GJGE_DEV_TOOLS
    registry.emplace<InspectorIntegration>(entity, name);
#endif
    registry.emplace<comp::Transform2D>(entity);
    return entity;
}
auto ge::create_child(entt::registry& registry, entt::entity parent, const char* name) -> entt::entity {
    const auto entity = create(registry, name);
    add_relation(registry, parent, entity);
    return entity;
}
