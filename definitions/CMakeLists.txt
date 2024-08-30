add_library(CONFIG dummy.cpp global_defines.hpp)
target_include_directories(CONFIG PUBLIC ${CMAKE_SOURCE_DIR}/game/inc ${CMAKE_SOURCE_DIR}/engine/inc ${CMAKE_SOURCE_DIR}/definitions)
target_link_libraries(CONFIG Entt raylib Imgui Enet)
target_compile_options(CONFIG PRIVATE ${COMPILE_FLAGS})