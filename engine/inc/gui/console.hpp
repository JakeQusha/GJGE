#pragma once
#include "logs.hpp"
#include <functional>
#include <vector>
#include <entt.hpp>
#include <imgui.h>

namespace ge {
using LogCallback = const std::function<void(LogLevel, const char*)>&;
using Params = std::vector<std::string>;
template <typename T>
concept ConsoleCommand = requires(const char* command, entt::registry& registry, LogCallback add_log, Params& params) {
    { T::name } -> std::convertible_to<const char*>;
    { T::execute(params, registry, add_log) };
    { std::is_empty_v<T> };
};

template <ConsoleCommand... AvailableCommand>
class Console {
    entt::registry& registry;
    std::vector<Log> log_history;
    int history_pos = -1;
    std::vector<std::string> commands_history;
    static constexpr auto input_buffer_size = 512;
    char input_buffer[input_buffer_size] = {};
    ImGuiTextFilter filter;
    bool reclaim_focus = false;
    bool auto_scroll = true;
    bool show_logs = true;
    struct {
        bool show_err = true;
        bool show_info = true;
        bool show_debug = false;
    } log_filter;

public:
    static constexpr auto name = "Console";
    bool is_open = false;

    void add_log(LogLevel ll, const char* message) { log_history.emplace_back(ll, message, false); }

    explicit Console(entt::registry& registry) : registry(registry) {}

    void clean_logs() { log_history.clear(); }

private:
    void add_log_command(LogLevel ll, const char* message) { log_history.emplace_back(ll, message, true); }

    auto text_edit(ImGuiInputTextCallbackData* data) -> int {
        switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion:
            // TODO
            break;
        case ImGuiInputTextFlags_CallbackHistory:
            if (data->EventKey == ImGuiKey_UpArrow && history_pos < static_cast<long>(commands_history.size()) - 1) {
                history_pos++;
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, commands_history.at(commands_history.size() - history_pos - 1).c_str());
                break;
            }
            if (data->EventKey == ImGuiKey_DownArrow) {
                if (history_pos != -1) {
                    history_pos--;
                    if (history_pos == -1) {
                        data->DeleteChars(0, data->BufTextLen);
                        break;
                    }
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, commands_history.at(commands_history.size() - history_pos - 1).c_str());
                }
            }
            break;
        default:
            break;
        }
        return 0;
    }

    static auto parse_command(std::unique_ptr<char[]>& command, const char* command_w_params, Params& params) -> bool {
        const char* first_space = strchr(command_w_params, ' ');
        if (!first_space) {
            command = std::make_unique<char[]>(strlen(command_w_params) + 1);
            strcpy(command.get(), command_w_params);
            return true;
        }
        auto len = static_cast<size_t>(first_space - command_w_params);
        command = std::make_unique<char[]>(len + 1);
        memcpy(command.get(), command_w_params, len);
        command[len] = '\0';
        command_w_params += len + 1;
        std::string param;
        bool in_quotes = false;
        char next;
        while (*command_w_params) {
            switch (*command_w_params) {
            case '\\':
                next = *(command_w_params + 1);
                switch (next) {
                case '\\':
                case '"':
                case ' ':
                    param.push_back(*(command_w_params + 1));
                    command_w_params++;
                    break;
                case 'n':
                    param.push_back('\n');
                    command_w_params++;
                    break;
                default:
                    if (isdigit(next)) {
                        param.push_back(static_cast<char>(next - '0'));
                        command_w_params++;
                        break;
                    }
                    return false;
                }
                break;
            case ' ':
                if (in_quotes) {
                    param.push_back(*command_w_params);
                    break;
                }
                while (*(command_w_params + 1) == ' ') {
                    command_w_params++;
                }
                params.emplace_back(std::move(param));
                param.clear();
                break;
            case '"':
                in_quotes = !in_quotes;
                break;
            default:
                param.push_back(*command_w_params);
            }
            command_w_params++;
        }
        if (in_quotes) {
            return false;
        }
        if (!param.empty()) {
            params.emplace_back(std::move(param));
        }
        return true;
    }

    void execute_command(const char* command_w_params) {
        commands_history.emplace_back(command_w_params);
        add_log_command(ge::LogLevel::NONE, std::format("# {}", command_w_params).c_str());
        Params params;
        std::unique_ptr<char[]> command;
        if (!parse_command(command, command_w_params, params)) {
            add_log_command(ge::LogLevel::ERR, "Syntax Error");
            return;
        }
        add_log(ge::LogLevel::DEBUG, command.get());
        // log params
        for (const auto& param : params) {
            add_log(ge::LogLevel::DEBUG, std::format("Param: {}", param).c_str());
        }
        // check built-ins
        if (!strcmp("help", command.get())) {
            add_log_command(LogLevel::NONE, "Available commands:");
            (
                [&]() {
                    add_log_command(LogLevel::NONE, std::format(" - {}", AvailableCommand::name).c_str());
                }(),
                ...);
            return;
        }
        // check commands
        if (!([&]() {
                if (!strcmp(command.get(), AvailableCommand::name)) {
                    AvailableCommand::execute(
                        params, registry, [this](LogLevel ll, const char* message) { add_log_command(ll, message); });
                    return true;
                }
                return false;
            }() ||
              ...)) {
            add_log_command(ge::LogLevel::ERR, std::format("Command Not Found: {}", command.get()).c_str());
        }
    }

public:
    void empty_logger() {
        while (!logger.logs.empty()) {
            log_history.emplace_back(std::move(logger.logs.front()));
            logger.logs.pop();
        }
    }
    void draw_gui() { // NOLINT
        if (!is_open) {
            return;
        }
        using namespace ImGui;
        // init
        ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
        if (!Begin(name, &is_open, ImGuiWindowFlags_None)) [[unlikely]] {
            End();
            return;
        }
        // top row
        if (Button("Clear")) {
            clean_logs();
        }
        SameLine();
        Checkbox("Show Logs", &show_logs);
        SameLine();
        if (BeginPopup("LogFilter")) {
            Checkbox("Show Info", &log_filter.show_info);
            Checkbox("Show Error", &log_filter.show_err);
            Checkbox("Show Debug", &log_filter.show_debug);
            EndPopup();
        }
        if (Button("Filter")) {
            OpenPopup("LogFilter");
        }
        SameLine();
        filter.Draw("Text Filter", 180);
        // text area
        Separator();
        if (BeginChild("logs", ImVec2(0, -GetFrameHeightWithSpacing() - GetStyle().ItemSpacing.y),
                       ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar)) {
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
            for (const auto& log : log_history) {
                if ((!show_logs && !log.command_caused) || !filter.PassFilter(log.log)) {
                    continue;
                }

                switch (log.ll) {
                case LogLevel::ERR:
                    if (!log_filter.show_err) {
                        continue;
                    }
                    TextColored(ImVec4(1, 0, 0, 1), "%s", log.log);
                    break;
                case LogLevel::INFO:
                    if (!log_filter.show_info) {
                        continue;
                    }
                    TextColored(ImVec4(0, 1, 1, 1), "%s", log.log);
                    break;
                case LogLevel::DEBUG:
                    if (!log_filter.show_debug) {
                        continue;
                    }
                    TextColored(ImVec4(1, 1, 1, .5), "%s", log.log);
                    break;
                case LogLevel::NONE:
                    TextUnformatted(log.log);
                    break;
                }
            }
            if (auto_scroll && GetScrollY() >= GetScrollMaxY()) {
                SetScrollHereY(1.0f);
            }
            ImGui::PopStyleVar();
            EndChild();
        }
        // bottom row
        Separator();
        if (reclaim_focus) {
            SetKeyboardFocusHere();
            reclaim_focus = false;
        }
        auto input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
                                ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        auto summit = [&]() {
            if (*input_buffer) {
                history_pos = -1;
                add_log(LogLevel::DEBUG, input_buffer);
                execute_command(input_buffer);
            }
            *input_buffer = '\0';
            reclaim_focus = true;
        };
        if (InputText(
                "##", input_buffer, sizeof input_buffer, input_text_flags,
                [](ImGuiInputTextCallbackData* data) -> int {
                    return static_cast<Console*>(data->UserData)->text_edit(data);
                },
                static_cast<void*>(this))) {
            summit();
        }
        SameLine();
        if (Button("Submit")) {
            summit();
        }
        SameLine();
        Checkbox("Auto Scroll", &auto_scroll);
        End();
    }
};
} // namespace ge