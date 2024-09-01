file(GLOB_RECURSE GJGE_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(GJGE ${GJGE_SOURCES})
target_link_libraries(GJGE Entt raylib Imgui Enet)
target_include_directories(GJGE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
target_compile_options(GJGE PRIVATE ${COMPILE_FLAGS})