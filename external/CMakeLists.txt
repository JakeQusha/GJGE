#raylib
set(RAYLIB_VERSION 6.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# this magic formula silences warnings from raylib
set_target_properties(raylib PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:raylib,INTERFACE_INCLUDE_DIRECTORIES>)

#raylib-cpp (header-only C++ wrapper that sits on top of raylib)
set(RAYLIB_CPP_VERSION 6.0.2)
include(FetchContent)
FetchContent_Declare(
        raylib_cpp
        URL https://github.com/RobLoach/raylib-cpp/archive/refs/tags/v${RAYLIB_CPP_VERSION}.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(raylib_cpp)
if (NOT raylib_cpp_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib_cpp)
endif()
# header-only: expose the headers and pull in raylib itself, don't build examples/tests
add_library(RaylibCpp INTERFACE)
target_include_directories(RaylibCpp INTERFACE ${raylib_cpp_SOURCE_DIR}/include)
target_link_libraries(RaylibCpp INTERFACE raylib)
# silence -Wconversion/-Wpedantic coming from the wrapper headers
set_target_properties(RaylibCpp PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:RaylibCpp,INTERFACE_INCLUDE_DIRECTORIES>)

#EnTT
add_library(Entt INTERFACE)
target_include_directories(Entt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/entt/include)
set_target_properties(Entt PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Entt,INTERFACE_INCLUDE_DIRECTORIES>)

#imgui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(IMGUI_RAYLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rlimgui)
add_library(Imgui STATIC ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp ${IMGUI_RAYLIB_DIR}/rlImGui.cpp)
target_link_libraries(Imgui raylib)
target_include_directories(Imgui PUBLIC ${IMGUI_DIR} ${IMGUI_RAYLIB_DIR})
set_target_properties(Imgui PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Imgui,INTERFACE_INCLUDE_DIRECTORIES>)

#enet
if(GJGE_FEATURE_NETWORKING)
    set(ENET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/enet)
    add_library(Enet STATIC ${ENET_DIR}/impl/enet.cpp)
    target_include_directories(Enet PRIVATE ${ENET_DIR}/impl)
    target_include_directories(Enet PUBLIC ${ENET_DIR}/include)
    set_target_properties(Enet PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:Enet,INTERFACE_INCLUDE_DIRECTORIES>)
endif ()

#box2d
if(GJGE_FEATURE_BOX2D)
    add_subdirectory(box2d)
endif ()
include(FetchContent)