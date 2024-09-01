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

    auto img = LoadImage("./resources/bill.png");
    auto img2 = LoadImage("./resources/job.png");
    auto txt = LoadTextureFromImage(img);
    auto txt2 = LoadTextureFromImage(img2);
    auto registry = entt::registry();
    auto toolbox = ge::Toolbox<
            Console_t,
            Inspector_t
    >(registry);
    auto &console = std::get<Console_t>(toolbox.windows);
    auto &inspector = std::get<Inspector_t>(toolbox.windows);
    auto &key_manager = registry.ctx().emplace<ge::KeyManager>();
    auto &asset_manager = registry.ctx().emplace<ge::AssetManager>();
    auto mt = ge::MultiTexture(txt);
    auto mt2 = ge::MultiTexture(txt2);
    asset_manager.add<ge::MultiTexture>("bill",std::move(mt));
    asset_manager.add<ge::MultiTexture>("job",std::move(mt2));
    auto entity = registry.create();
    registry.emplace<Dead>(entity);
    registry.emplace<Alive>(entity);
    registry.emplace<ge::comp::Transform2D>(entity);
    registry.emplace<ge::comp::Sprite>(entity);
    auto &sprite = registry.get<ge::comp::Sprite>(entity);
    sprite.texture = asset_manager.get<ge::MultiTexture>("bill");
    key_manager.assign_key(KEY_D, "essing");
    key_manager.subscribe(ge::KeyboardEvent::PRESS, "essing", [&]() {
        console.add_log(ge::LogLevel::INFO, "TEST");
    });
    key_manager.assign_key(KEY_Q, "essing");
    //auto &asset_manager = registry.ctx().emplace<ge::AssetManager<Texture,Sound>>();
    auto stary = ge::create(registry,"stary");
    auto mlody = ge::create(registry,"mlody");
    auto mlody2 = ge::create(registry,"mlody2");
    auto niemowle = ge::create(registry,"mniemowle");
    ge::add_relation(registry,stary,mlody2);
    ge::add_relation(registry,mlody,niemowle);
    ge::add_relation(registry,stary,mlody);
    while (!WindowShouldClose()) {
        console.empty_logger();
        BeginDrawing();
        ClearBackground(GREEN);
        DrawFPS(15, 15);
//        DrawText(std::format("{}",huj).c_str(),100,100,20,RED);
        rlImGuiBegin();
        ImGui::ShowDemoWindow();
        toolbox.draw_gui();
        rlImGuiEnd();
        EndDrawing();
        ge::notify_keyboard_press_system(key_manager);
        ge::calculate_global_transform(registry);
        ge::comp::draw_sprites(registry);
    }


    exit:
    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();
}
