#pragma once
#include <imgui.h>

struct Alive {
    static constexpr auto name = "Alive";

    static void inspect() { ImGui::Text("Entity Alive :>"); }
};
struct Dead {
    static constexpr auto name = "ded";

    static void inspect() { ImGui::Text("Entity ded :>"); }
};