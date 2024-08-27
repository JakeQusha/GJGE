#pragma once

#include <imgui_internal.h>
#include <entt.hpp>
#include <imgui.h>
#include <optional>
#include "entity_managment.hpp"
#include <variant>
#include "logs.hpp"

namespace ge {
    struct InspectorIntegration {
        std::string debug_name;
    };

    template<typename T>
    concept InspectableComponent = requires(T t, entt::registry &registry, entt::entity entity) {
        { T::name } -> std::convertible_to<const char *>;
        { t.inspect(registry, entity) };
    } || requires() {
        { T::name } -> std::convertible_to<const char *>;
        { T::inspect() };
        { std::is_empty_v<T> };
    };

    template<InspectableComponent T>
    void inspect(T &t, entt::registry &registry, entt::entity entity) {
        if constexpr (std::is_empty_v<T>) {
            T::inspect();
        } else {
            t.inspect(registry, entity);
        }
    }

    template<InspectableComponent... Component>
    struct Inspector {
        static constexpr auto name = "Inspector";
        static constexpr auto EntityDragDropTypeName = "Entity";
        static constexpr auto ComponentDragdropTypeName = "Component";
        std::variant<Component..., std::monostate> generated_component = std::monostate{};
        entt::registry &registry;
        std::array<bool, sizeof...(Component)> component_filter{};
        ImGuiTextFilter filter;
        bool is_open = false;

        explicit Inspector(entt::registry &registry) : registry(registry) {}

        std::optional<entt::entity> current_entity;

        void draw_gui() {
            if (!is_open) {
                return;
            }
            ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin(name, &is_open, ImGuiWindowFlags_MenuBar))[[unlikely]] {
                ImGui::End();
                return;
            }
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Entity")) {
                    if (ImGui::MenuItem("New Entity")) {
                        ge::logger.add_log(ge::LogLevel::INFO, "New Entity Created");
                        const auto entity = registry.create();
                        registry.emplace<InspectorIntegration>(entity, "New entity");
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Components")) {
                    auto i = 0u;
                    ([&]() { ImGui::Checkbox(Component::name, &component_filter[i++]); }(), ...);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 0)); // Optional: reduce spacing between items

            // Split main window horizontally

            ImGui::BeginChild("Entity List", ImVec2{220, 0},
                              ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX | ImGuiChildFlags_NavFlattened);
            display_entity_list();
            ImGui::EndChild();

            ImGui::SameLine();

            // Begin right column
            ImGui::BeginGroup();
            ImVec2 rightColumnSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
            if (!current_entity) {
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightColumnSize.x / 2 - 70);
                ImGui::Text("No entity selected");
                ImGui::PopStyleVar();
                ImGui::EndGroup();
                ImGui::End();
                return;
            }
            const auto entity_list_size = ImVec2(rightColumnSize.x, rightColumnSize.y * 3.f / 5.f);
            display_entity_info(entity_list_size);

            // Bottom part of the right column
            display_component_creator();

            ImGui::PopStyleVar();
            ImGui::EndGroup();
            ImGui::End();
        }

    private:
        void display_entity_list() {
            auto i = 0u;
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);
            if (ImGui::InputTextWithHint("##", "Search", filter.InputBuf, sizeof filter.InputBuf,
                                         ImGuiInputTextFlags_EscapeClearsAll)) {
                filter.Build();
            }
            ImGui::SeparatorText("Entity List");
            //ImGui::BeginChild("List", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_None);
            if (ImGui::BeginTable("##List", 1, ImGuiTableFlags_RowBg)) {
                for (auto entity: registry.view<entt::entity>()) {
                    if (registry.all_of<ge::comp::Child>(entity)) {
                        continue;
                    }
                    display_entity_list_entry(entity);
                }

                if (ImGui::BeginPopupContextWindow()) {
                    if (ImGui::MenuItem("Create new entity")) {
                        ge::logger.add_log(ge::LogLevel::INFO, "New Entity Created");
                        const auto entity = registry.create();
                        registry.emplace<InspectorIntegration>(entity, "New entity");
                    }
                    ImGui::EndPopup();
                }
                ImGui::EndTable();
            }
        }

        void display_component_creator() {

            ImGui::PushID((int) generated_component.index());
            ImGui::BeginChild("Component creator", ImVec2(0, 0), 1);
            ImGui::SeparatorText("Component creator");

            static const char *chosen_component_name = "None";
            ImVec2 text_size = ImGui::CalcTextSize(chosen_component_name);

            ImGui::Text("Chosen type: ");
            ImGui::SameLine();
            if (ImGui::Button(chosen_component_name, ImVec2(text_size.x + 10, 0))) {
                ImGui::OpenPopup(chosen_component_name);
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload(ComponentDragdropTypeName, &generated_component, sizeof generated_component);
                ImGui::Text(chosen_component_name);
                ImGui::EndDragDropSource();
            }

            ImVec2 button_pos = ImGui::GetItemRectMin();
            ImVec2 button_size = ImGui::GetItemRectSize();
            ImGui::SetNextWindowPos(ImVec2(button_pos.x, button_pos.y + button_size.y), ImGuiCond_Appearing);

            if (ImGui::BeginPopup(chosen_component_name)) {
                uint32_t i = 0;
                iterate_components([&]<InspectableComponent Comp>() {
                    if (ImGui::MenuItem(Comp::name, NULL, generated_component.index() == i)) {
                        generated_component = Comp{};
                        chosen_component_name = Comp::name;

                        ImGui::CloseCurrentPopup();
                    }
                    i++;
                });
                ImGui::EndPopup();
            }

            ImGui::SeparatorText("Component data");

            std::visit(entt::overloaded{
                               [&](std::monostate) { ImGui::Text("No component selected"); },
                               [&](auto &component) { inspect(component, registry, entt::null); },
                       },
                       generated_component);

            ImGui::EndChild();
            ImGui::PopID();
        }

        void display_entity_list_entry(entt::entity entity) {
            if (!registry.all_of<InspectorIntegration>(entity)) {
                registry.emplace<InspectorIntegration>(entity, "Unknown");
            }
            auto &ii = registry.get<InspectorIntegration>(entity);
            const auto entity_name = std::format("{} ({})", ii.debug_name, static_cast<uint32_t>(entity));
            if (!filter.PassFilter(entity_name.c_str())) {
                return;
            }
            auto i = 0u;
            if (!([&]() { return !component_filter[i++] || registry.all_of<Component>(entity); }() && ...)) {
                return;
            }
            auto *const parent_comp = registry.try_get<ge::comp::Parent>(entity);
            ImGui::PushID(static_cast<int>(entity));
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                            ImGuiTreeNodeFlags_NavLeftJumpsBackHere;
            if (entity == current_entity) {
                tree_flags |= ImGuiTreeNodeFlags_Selected;
            }
            tree_flags |= parent_comp ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
            bool open = ImGui::TreeNodeEx("##", tree_flags, "%s", entity_name.c_str());
            if (ImGui::IsItemFocused()) {
                current_entity = entity;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::SeparatorText(entity_name.c_str());
                iterate_components([&]<InspectableComponent Comp>() {
                    if (!registry.all_of<Comp>(entity)) {
                        return;
                    }
                    ImGui::Text(Comp::name);
                });
                ImGui::EndTooltip();
            }
            if (open) {
                if(parent_comp){
                    for (auto child: parent_comp->children) {
                        display_entity_list_entry(child);
                    }
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        void iterate_components(auto &&f) { (f.template operator()<Component>(), ...); }

        void display_entity_info(ImVec2 size) {
            entt::entity entity = *current_entity;

//            ImGui::BeginChild("Entity Inspector", size, 1);if (ImGui::BeginDragDropTarget()) {
//                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(ComponentDragdropTypeName)) {
//                    uint32_t i = 0u;
//                    (
//                            [&]() {
//                                fmt::println("name: {}", Component::name);
//                                if (i++ != generated_component.index() ||
//                                    std::holds_alternative<std::monostate>(generated_component)) {
//                                    return;
//                                }
//
//                                an::emplace<Component>(*registry, entity, std::get<Component>(generated_component));
//                            }(),
//                            ...);
//                }
//                ImGui::EndDragDropTarget();
//            }
            if (ImGui::Button("Delete")) {
                ge::kill(registry, entity);
                current_entity = std::nullopt;
                return;
            }

            ImGui::SeparatorText("Entity data");

            ImGui::BeginGroup();
            ImGui::Text("Entity: %d", (int) entity);
            if (auto *ii = registry.try_get<InspectorIntegration>(entity)) {
                ImGui::Text("Name: %s", ii->debug_name.c_str());
            }
            ImGui::EndGroup();

            display_components(entity);

            //ImGui::EndChild();

//

        }

        void display_components(entt::entity entity) {
            ImGui::SeparatorText("Components");

            iterate_components([&]<InspectableComponent Comp>() {
                if (!registry.all_of<Comp>(entity)) {
                    return;
                }

                ImGui::PushID(Comp::name);

                bool header_open = ImGui::CollapsingHeader(Comp::name);

                // Create a popup menu on right-click
                if (ImGui::BeginPopupContextItem("component_context_menu")) {
                    if (ImGui::MenuItem("Delete")) {
                        registry.remove<Comp>(entity);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                if (header_open) {
                    if constexpr (std::is_empty_v<Comp>) {
                        Comp::inspect();
                    } else {
                        auto &component = registry.get<Comp>(entity);
                        component.inspect(registry, entity);
                    }
                }

                ImGui::PopID();
            });
        }
    };

} // namespace an