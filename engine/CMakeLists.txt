file(GLOB_RECURSE GJGE_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(GJGE ${GJGE_SOURCES}
        inc/gui/toolbox.hpp
        inc/gui/console.hpp
        inc/gui/console_commands.hpp)
target_link_libraries(GJGE Entt raylib Imgui Enet)
target_include_directories(GJGE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/assets
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/gui
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/components
)
target_compile_options(GJGE PRIVATE ${COMPILE_FLAGS})