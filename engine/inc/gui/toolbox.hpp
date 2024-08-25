#pragma once

#include <functional>
#include <imgui.h>
namespace ge {
    template<typename T>
    concept Window = requires(T t){
        { T::name } -> std::convertible_to<const char *>;
        { t.is_open } -> std::convertible_to<bool>;
        { t.draw_gui() };
    };
    template<Window... Win>
    struct Toolbox {
        static constexpr auto name = "ToolBox";
        std::tuple<Win...> windows;
        Toolbox() = delete;
        Toolbox(const Toolbox &) = delete;
        Toolbox(Toolbox &&) = delete;
        explicit Toolbox(entt::registry &registry):windows{Win(registry)...}{
        }
        void draw_gui(){
            std::apply([](auto &...win){(win.draw_gui(),...);},windows);
            using namespace ImGui;
            if (!Begin(name, nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings))[[unlikely]] {
                End();
                return;
            }
            std::apply([](auto &...win){
                ([&](){
                    Checkbox(win.name, &win.is_open);
                }(),...);
            },windows);
            End();

        }
    };
}