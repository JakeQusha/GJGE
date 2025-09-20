file(GLOB_RECURSE GJGE_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(GJGE ${GJGE_SOURCES})
target_link_libraries(GJGE Entt raylib Imgui)
if(GJGE_FEATURE_NETWORKING)
    target_link_libraries(GJGE Enet)
endif()
if(GJGE_FEATURE_BOX2D)
    target_link_libraries(GJGE box2d::box2d)
endif()
target_include_directories(GJGE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
target_compile_options(GJGE PRIVATE ${COMPILE_FLAGS})