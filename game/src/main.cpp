#include <raylib.h>
#include <print>
#include <rlImGui.h>
#include <imgui.h>
#include <entt.hpp>
#include "asset_manager.hpp"
#include "keyinput.hpp"
#include "toolbox.hpp"
#include "typdefs.hpp"


static void setup_raylib() {
    const auto display = GetCurrentMonitor();
    const int screen_width = GetMonitorWidth(display);
    const int screen_height = GetMonitorHeight(display);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    std::println("Resolution is: {}x{}", screen_width, screen_height);
    InitWindow(screen_width, screen_height, "Hello World");
    InitAudioDevice();
    SetTargetFPS(60);
}
auto main() -> int {
    setup_raylib();
    SetExitKey(KEY_DELETE);
    rlImGuiSetup(true);
    auto registry = entt::registry();
    auto toolbox = ge::Toolbox<
            Console_t,
            Inspector_t
    >(registry);
    auto &console = std::get<Console_t>(toolbox.windows);
    auto &inspector = std::get<Inspector_t>(toolbox.windows);
    auto &key_manager = registry.ctx().emplace<ge::KeyManager>();
    auto entity = registry.create();
    registry.emplace<Alive>(entity);
    registry.get<Alive>(entity);
    key_manager.assign_key(KEY_D, "essing");
    key_manager.subscribe(ge::KeyboardEvent::PRESS, "essing", [&]() {
        console.add_log(ge::LogLevel::INFO, "TEST");
    });
    key_manager.assign_key(KEY_Q, "essing");
    //auto &asset_manager = registry.ctx().emplace<ge::AssetManager<Texture,Sound>>();
    auto stary = registry.create();
    auto mlody = registry.create();
    auto mlody2 = registry.create();
    ge::comp::add_relation(registry,stary,mlody);
    ge::comp::add_relation(registry,stary,mlody2);
    registry.emplace<ge::InspectorIntegration>(stary,"stary");
    registry.emplace<ge::InspectorIntegration>(mlody2,"mlody2");
    registry.emplace<ge::InspectorIntegration>(mlody,"mlody");

    while (!WindowShouldClose()) {
        console.empty_logger();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(15, 15);
//        DrawText(std::format("{}",huj).c_str(),100,100,20,RED);
        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        toolbox.draw_gui();
        rlImGuiEnd();
        EndDrawing();
        ge::notify_keyboard_press_system(key_manager);
    }


    exit:
    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();
}
