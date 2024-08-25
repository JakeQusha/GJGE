#include "console.hpp"
#include <raylib.h>
#include <print>
#include <rlImGui.h>
#include <imgui.h>
#include <entt.hpp>
#include "asset_manager.hpp"
#include "keyinput.hpp"
#include "toolbox.hpp"
#include "inspector.hpp"

#include "console_commands.hpp"

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

struct Alive {
    static constexpr auto name = "Alive";

    static void inspect() { ImGui::Text("Entity Alive :>"); }
};

auto main() -> int {
    setup_raylib();
    SetExitKey(KEY_DELETE);
    rlImGuiSetup(true);
    auto registry = entt::registry();
    auto toolbox = ge::Toolbox<
            ge::Console<ge::hi_command, ge::echo_command>,
            ge::Inspector<Alive>
    >(registry);
    auto &console = std::get<0>(toolbox.windows);
    auto &inspector = std::get<1>(toolbox.windows);
    auto &key_manager = registry.ctx().emplace<ge::KeyManager>();
    auto entity = registry.create();
    registry.emplace<Alive>(entity);
    key_manager.subscribe(ge::KeyboardEvent::PRESS, KEY_Q, [&]() {
        console.add_log(ge::LogLevel::INFO, "TEST");
    });

    //auto &asset_manager = registry.ctx().emplace<ge::AssetManager<Texture,Sound>>();


    while (!WindowShouldClose()) {

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
