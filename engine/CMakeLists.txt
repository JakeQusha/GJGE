set(GJGE_SOURCES
        src/entity_management.cpp
        src/keyinput.cpp
        src/scene.cpp
        src/template.cpp
        src/assets/animation.cpp
        src/assets/assets_loader.cpp
        src/components/collision2D.cpp
        src/components/relations.cpp
        src/components/sprite.cpp
        src/components/transform2D.cpp
)
add_library(GJGE ${GJGE_SOURCES})
target_link_libraries(GJGE Entt RaylibCpp Imgui)
if(GJGE_FEATURE_NETWORKING)
    target_link_libraries(GJGE Enet)
endif()
target_include_directories(GJGE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
target_compile_options(GJGE PRIVATE ${COMPILE_FLAGS})
