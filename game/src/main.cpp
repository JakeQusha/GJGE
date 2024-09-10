#include <raylib.h>
#include <print>
#include <rlImGui.h>
#include <imgui.h>
#include <entt.hpp>
#include "assets/asset_manager.hpp"
#include "keyinput.hpp"
#include "gui/toolbox.hpp"
#include "components/sprite.hpp"
#include "typedefs.hpp"
#include "templates.hpp"
#include "template.hpp"
#include "components/collision2D.hpp"

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
    auto &asset_manager = registry.ctx().emplace<ge::AssetManager>();
    asset_manager.add<ge::MultiTexture>("blue",ge::LoadMultiTexture("./resources/blue.png"));
    asset_manager.add<ge::MultiTexture>("orange",ge::LoadMultiTexture("./resources/orange.png"));

    generate_templates(registry);

    auto entity = registry.create();
    registry.emplace<Dead>(entity);
    registry.emplace<Alive>(entity);
    registry.emplace<ge::comp::Transform2D>(entity);
    key_manager.assign_key(KEY_D, "essing");

    key_manager.subscribe(ge::KeyboardEvent::PRESS, "essing", [&]() {
        console.add_log(ge::LogLevel::INFO, "TEST");
    });

    key_manager.assign_key(KEY_Q, "essing");

    ge::instantiate_template(registry, "stary");
    ge::instantiate_template(registry, "stary");
    while (!WindowShouldClose()) {
        console.empty_logger();
        BeginDrawing();
        ClearBackground(GREEN);
        DrawFPS(15, 15);
        ge::draw_debug_colliders(registry);
        ge::evaluate_AABB_Collisions(registry);
        ge::draw_sprites(registry);
        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        toolbox.draw_gui();
        rlImGuiEnd();
        EndDrawing();
        ge::notify_keyboard_press_system(key_manager);
        ge::calculate_global_transform(registry);
    }


    exit:
    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();
}
