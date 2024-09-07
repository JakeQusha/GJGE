#pragma once

#include <imgui_internal.h>
#include <entt.hpp>
#include <imgui.h>
#include <optional>
#include "entity_management.hpp"
#include <variant>
#include "logs.hpp"
#include "template.hpp"

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
        std::variant<Component..., std::monostate> generated_component = std::monostate{};
        entt::registry &registry;
        std::array<bool, sizeof...(Component)> component_filter{};
        ImGuiTextFilter filter_list;
        ImGuiTextFilter filter_comp;
        bool is_creator_open = false;
        bool is_open = false;
        char temp_name[32]{};
        bool wait = false;

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

                    if (ImGui::BeginMenu("New Entity From Template")) {
                        for (auto &asset: registry.ctx().get<AssetManager>().get_all<ge::Template>()) {
                            if (ImGui::MenuItem(asset.first)) {
                                ge::instantiate_template(registry, asset.first);
                            }
                        }
                        ImGui::EndMenu();
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
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 0.)); // Optional: reduce spacing between items

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
            display_entity_info();

            // Bottom part of the right column
            display_component_creator();

            ImGui::PopStyleVar();
            ImGui::EndGroup();
            ImGui::End();
        }

    private:
        void display_entity_list() {
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);
            if (ImGui::InputTextWithHint("##", "Search", filter_list.InputBuf, sizeof filter_list.InputBuf,
                                         ImGuiInputTextFlags_EscapeClearsAll)) {
                filter_list.Build();
            }
            ImGui::SeparatorText("Entity List");
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
            if (!is_creator_open) return;
            if (wait) {
                wait = false;
                return;
            }
            ImGui::BeginChild("Component creator", ImVec2(0, 0), ImGuiChildFlags_Border);
            ImGui::SeparatorText("Component creator");

            static const char *chosen_component_name = "None";
            ImVec2 text_size = ImGui::CalcTextSize(chosen_component_name);
            ImGui::BeginChild("type", ImVec2(-120, 20), ImGuiWindowFlags_None);
            ImGui::Text("Chosen type: ");
            ImGui::SameLine();
            if (ImGui::Button(chosen_component_name, ImVec2(text_size.x + 10, 0))) {
                ImGui::OpenPopup(chosen_component_name);
            }
            if (ImGui::BeginPopup(chosen_component_name)) {
                auto i = 0u;
                iterate_components([&]<InspectableComponent Comp>() {
                    if (ImGui::MenuItem(Comp::name, nullptr, generated_component.index() == i)) {
                        generated_component = Comp{};
                        chosen_component_name = Comp::name;

                        ImGui::CloseCurrentPopup();
                    }
                    i++;
                });
                ImGui::EndPopup();
            }
            ImGui::EndChild();
            ImGui::SameLine();
            if (ImGui::Button("Add Component", ImVec2(100, 20))) {
                entt::entity entity = *current_entity;
                std::visit([&](auto &&arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::monostate>) {
                        logger.add_log(LogLevel::ERR, "No component selected");
                        return;
                    }
                    emplace<T>(registry,entity, std::forward<decltype(arg)>(arg));
                }, generated_component);
            }

            ImGui::SeparatorText("Component data");
            std::visit(entt::overloaded{
                               [&](std::monostate) { ImGui::Text("No component selected"); },
                               [&](auto &component) { inspect(component, registry, entt::null); },
                       },
                       generated_component);

            ImGui::EndChild();
        }

        void display_entity_list_entry(entt::entity entity) {
            if (!registry.all_of<InspectorIntegration>(entity)) {
                registry.emplace<InspectorIntegration>(entity, "Unknown");
            }
            auto &ii = registry.get<InspectorIntegration>(entity);
            const auto entity_name = std::format("{} ({})", ii.debug_name, static_cast<uint32_t>(entity));
            if (!filter_list.PassFilter(entity_name.c_str())) {
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
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("child", &entity, sizeof entity);
                ImGui::Text("%s", entity_name.c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("child")) {
                    entt::entity child = *(entt::entity *) payload->Data;
                    auto *child_comp = registry.try_get<comp::Child>(child);
                    if (!child_comp) {
                        entt::entity test_e = entity;
                        while (true) {
                            auto *child_comp = registry.try_get<comp::Child>(test_e);
                            if (!child_comp) {
                                break;
                            }
                            test_e = child_comp->parent;
                            if (test_e == child) {
                                goto DnDEnd;
                            }
                        }
                        add_relation(registry, entity, child);
                        goto DnDEnd;
                    }
                    entt::entity parent = child_comp->parent;
                    if (parent != entity) {
                        entt::entity test_e = entity;
                        while (true) {
                            auto *child_comp = registry.try_get<comp::Child>(test_e);
                            if (!child_comp) {
                                break;
                            }
                            test_e = child_comp->parent;
                            if (test_e == child) {
                                goto DnDEnd;
                            }
                        }
                        remove_relation(registry, parent, child);
                        add_relation(registry, entity, child);
                        goto DnDEnd;
                    }
                    remove_relation(registry, parent, child);
                }
                DnDEnd:
                ImGui::EndDragDropTarget();
            }
            if (open) {
                if (parent_comp) {
                    for (auto child: parent_comp->children) {
                        display_entity_list_entry(child);
                    }
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        void iterate_components(auto &&f) { (f.template operator()<Component>(), ...); }

        void display_entity_info() {
            entt::entity entity = *current_entity;
            ImGui::BeginChild("Entity Inspector", ImVec2(0, (is_creator_open ? (ImGui::GetWindowHeight() * .7f) : 0)),
                              ImGuiChildFlags_Border,
                              ImGuiWindowFlags_None);
            ImGui::BeginGroup();

            ImGui::BeginChild("data", ImVec2(-70, 75), ImGuiChildFlags_None, ImGuiWindowFlags_None);
            ImGui::SeparatorText("Entity data");
            ImGui::Text("Entity: %d", static_cast<uint32_t>(entity));
            auto &ii = registry.get<InspectorIntegration>(entity);
            ImGui::Text("Name: %s", ii.debug_name.c_str());
            ImGui::Text("Template: None");
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("Buttons", ImVec2(70, 75), ImGuiChildFlags_None, ImGuiWindowFlags_None);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 5.));
            ImGui::SetCursorPosX(-10);
            ImGui::SeparatorText("Options");
            if (ImGui::BeginPopup("Rename")) {
                if (ImGui::InputText("##", temp_name, sizeof temp_name, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    logger.add_log(LogLevel::DEBUG,
                                   std::format(R"(Renamed "{}" to "{}")", ii.debug_name, temp_name).c_str());
                    ii.debug_name.assign(temp_name);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            ImGui::SetCursorPosX(10);
            if (ImGui::Button("Rename")) {
                memcpy(temp_name, ii.debug_name.c_str(), std::min(ii.debug_name.size() + 1, sizeof temp_name));
                ImGui::OpenPopup("Rename", ImGuiPopupFlags_None);
            }
            ImGui::SetCursorPosX(10);
            if (ImGui::Button("Delete")) {
                ge::kill(registry, entity);
                current_entity = std::nullopt;
                ImGui::PopStyleVar();
                ImGui::EndChild();
                ImGui::EndGroup();
                ImGui::EndChild();
                return;
            }
            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::EndGroup();
            ImGui::SeparatorText("Components");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.f, 5.f));
            if (ImGui::BeginChild("List comp",
                                  ImVec2(0, -ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y),
                                  ImGuiChildFlags_None,
                                  ImGuiWindowFlags_None)) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.));
                iterate_components([&]<InspectableComponent Comp>() {
                    if (!registry.all_of<Comp>(entity) || !filter_comp.PassFilter(Comp::name)) {
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
                            auto *component = registry.try_get<Comp>(entity);
                            if(component){
                                component->inspect(registry, entity);
                            }
                        }
                    }

                    ImGui::PopID();
                });
                ImGui::PopStyleVar();
                ImGui::EndChild();
            }
            ImGui::Separator();
            ImGui::BeginChild("Component data", ImVec2(-120, 0), ImGuiWindowFlags_None);
            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);
            if (ImGui::InputTextWithHint("##", "Search", filter_comp.InputBuf, sizeof filter_comp.InputBuf,
                                         ImGuiInputTextFlags_EscapeClearsAll)) {
                filter_comp.Build();
            }
            ImGui::EndChild();
            ImGui::SameLine();
            if (ImGui::Button(is_creator_open ? "Hide Creator" : "Open Creator", ImVec2(100, 0))) {
                is_creator_open = !is_creator_open;
                wait = true;
            }
            ImGui::PopStyleVar();
            ImGui::EndChild();

        }
    };

} // namespace ge