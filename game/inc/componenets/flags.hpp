#pragma once
#include <imgui.h>

struct Alive {
    static constexpr auto name = "Alive";

    static void inspect() { ImGui::Text("Entity Alive :>"); }
};
