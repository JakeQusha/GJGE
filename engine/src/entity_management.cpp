#include "entity_management.hpp"

void ge::kill(entt::registry &registry, entt::entity entity) {
    if(!registry.valid(entity)){
        return;
    }
    relations_call_on_delete_entity(registry,entity);
    registry.destroy(entity);
}
