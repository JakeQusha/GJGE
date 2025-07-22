#include "entity_management.hpp"
#include "components/transform2D.hpp"
#include "gui/inspector.hpp"
void ge::kill(entt::registry& registry, entt::entity entity) {
    if (!registry.valid(entity)) {
        return;
    }
    relations_call_on_delete_entity(registry, entity);
    registry.destroy(entity);
}

auto ge::create(entt::registry& registry, const char* name) -> entt::entity {
    const auto entity = registry.create();
    registry.emplace<InspectorIntegration>(entity, name);
    registry.emplace<comp::Transform2D>(entity);
    return entity;
}
