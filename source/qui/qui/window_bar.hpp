#pragma once

#include <algorithm>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "qui/theme.hpp"
#include "qui/typography.hpp"

namespace qui {

enum WindowResizeEdge {
    WindowResizeEdge_None = 0,
    WindowResizeEdge_Left = 1 << 0,
    WindowResizeEdge_Right = 1 << 1,
    WindowResizeEdge_Top = 1 << 2,
    WindowResizeEdge_Bottom = 1 << 3,
};

struct WindowBarState {
    bool dragging = false;
    bool resizing = false;
    bool resize_cursor_active = false;
    int resize_edges = WindowResizeEdge_None;
    ImVec2 drag_offset = ImVec2(0.0F, 0.0F);
    ImVec2 resize_cursor_start = ImVec2(0.0F, 0.0F);
    ImVec2 resize_window_pos_start = ImVec2(0.0F, 0.0F);
    ImVec2 resize_window_size_start = ImVec2(0.0F, 0.0F);
    ImVec2 min_window_size = ImVec2(720.0F, 450.0F);
};

inline WindowBarState &default_window_bar_state() {
    static WindowBarState state;
    return state;
}

inline bool title_bar_button(const char *id, const char *label, const ImVec2 &pos, const ImVec2 &size, bool danger = false) {
    ImGui::SetCursorScreenPos(pos);
    ImGui::PushID(id);

    const bool pressed = ImGui::InvisibleButton("button", size);
    const bool hovered = ImGui::IsItemHovered();
    const bool active = ImGui::IsItemActive();

    ImVec4 background = color::rgba(0x00, 0x00, 0x00, 0x00);
    if (danger && (hovered || active)) {
        background = active ? color::rgba(0xB8, 0x32, 0x28) : color::retina_dark::AccentRed;
    } else if (active) {
        background = color::rgba(0x3A, 0x3A, 0x3A);
    } else if (hovered) {
        background = color::rgba(0x2F, 0x2F, 0x2F);
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 max(pos.x + size.x, pos.y + size.y);
    draw_list->AddRectFilled(pos, max, ImGui::ColorConvertFloat4ToU32(background));

    const ImVec4 text_color = danger && (hovered || active) ? color::retina_dark::Text : color::retina_dark::TextDisabled;
    const ImVec2 text_size = ImGui::CalcTextSize(label);
    const ImVec2 text_pos(pos.x + (size.x - text_size.x) * 0.5F, pos.y + (size.y - text_size.y) * 0.5F - 1.0F);
    draw_list->AddText(text_pos, ImGui::ColorConvertFloat4ToU32(text_color), label);

    ImGui::PopID();
    return pressed;
}

inline int hit_test_resize_edges(GLFWwindow *window, const ImVec2 &window_size, float border_size) {
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);

    if (cursor_x < 0.0 || cursor_y < 0.0 || cursor_x > window_size.x || cursor_y > window_size.y) {
        return WindowResizeEdge_None;
    }

    int edges = WindowResizeEdge_None;
    if (cursor_x <= border_size) {
        edges |= WindowResizeEdge_Left;
    } else if (cursor_x >= window_size.x - border_size) {
        edges |= WindowResizeEdge_Right;
    }

    if (cursor_y <= border_size) {
        edges |= WindowResizeEdge_Top;
    } else if (cursor_y >= window_size.y - border_size) {
        edges |= WindowResizeEdge_Bottom;
    }

    return edges;
}

inline GLFWcursor *resize_cursor_for_edges(int edges) {
    static GLFWcursor *resize_ns = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    static GLFWcursor *resize_ew = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    static GLFWcursor *resize_nwse = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    static GLFWcursor *resize_nesw = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

    const bool left = (edges & WindowResizeEdge_Left) != 0;
    const bool right = (edges & WindowResizeEdge_Right) != 0;
    const bool top = (edges & WindowResizeEdge_Top) != 0;
    const bool bottom = (edges & WindowResizeEdge_Bottom) != 0;

    if ((left && top) || (right && bottom)) {
        return resize_nwse;
    } else if ((right && top) || (left && bottom)) {
        return resize_nesw;
    } else if (left || right) {
        return resize_ew;
    } else if (top || bottom) {
        return resize_ns;
    }

    return nullptr;
}

inline void set_resize_cursor(GLFWwindow *window, int edges) {
    const bool left = (edges & WindowResizeEdge_Left) != 0;
    const bool right = (edges & WindowResizeEdge_Right) != 0;
    const bool top = (edges & WindowResizeEdge_Top) != 0;
    const bool bottom = (edges & WindowResizeEdge_Bottom) != 0;

    if ((left && top) || (right && bottom)) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
    } else if ((right && top) || (left && bottom)) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
    } else if (left || right) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    } else if (top || bottom) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }

    glfwSetCursor(window, resize_cursor_for_edges(edges));
}

inline void update_window_resize(GLFWwindow *window, const ImVec2 &window_pos, const ImVec2 &window_size, WindowBarState &state, float border_size = 8.0F) {
    (void)window_pos;
    (void)window_size;

    if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE) {
        state.resizing = false;
        return;
    }

    int current_width = 0;
    int current_height = 0;
    glfwGetWindowSize(window, &current_width, &current_height);

    const int hovered_edges = hit_test_resize_edges(window, ImVec2(static_cast<float>(current_width), static_cast<float>(current_height)), border_size);
    if (hovered_edges != WindowResizeEdge_None || state.resizing) {
        set_resize_cursor(window, state.resizing ? state.resize_edges : hovered_edges);
        state.resize_cursor_active = true;
    } else if (state.resize_cursor_active) {
        glfwSetCursor(window, nullptr);
        state.resize_cursor_active = false;
    }

    if (!state.resizing && hovered_edges != WindowResizeEdge_None && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        int window_x = 0;
        int window_y = 0;
        double cursor_x = 0.0;
        double cursor_y = 0.0;

        glfwGetWindowPos(window, &window_x, &window_y);
        glfwGetWindowSize(window, &current_width, &current_height);
        glfwGetCursorPos(window, &cursor_x, &cursor_y);

        state.dragging = false;
        state.resizing = true;
        state.resize_edges = hovered_edges;
        state.resize_cursor_start = ImVec2(static_cast<float>(window_x) + static_cast<float>(cursor_x), static_cast<float>(window_y) + static_cast<float>(cursor_y));
        state.resize_window_pos_start = ImVec2(static_cast<float>(window_x), static_cast<float>(window_y));
        state.resize_window_size_start = ImVec2(static_cast<float>(current_width), static_cast<float>(current_height));
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        state.resizing = false;
        state.resize_edges = WindowResizeEdge_None;
    }

    if (!state.resizing) {
        return;
    }

    double cursor_x = 0.0;
    double cursor_y = 0.0;
    int window_x = 0;
    int window_y = 0;
    glfwGetWindowPos(window, &window_x, &window_y);
    glfwGetCursorPos(window, &cursor_x, &cursor_y);

    const float cursor_screen_x = static_cast<float>(window_x) + static_cast<float>(cursor_x);
    const float cursor_screen_y = static_cast<float>(window_y) + static_cast<float>(cursor_y);
    const float delta_x = cursor_screen_x - state.resize_cursor_start.x;
    const float delta_y = cursor_screen_y - state.resize_cursor_start.y;

    float next_x = state.resize_window_pos_start.x;
    float next_y = state.resize_window_pos_start.y;
    float next_width = state.resize_window_size_start.x;
    float next_height = state.resize_window_size_start.y;
    bool moved_window = false;

    if ((state.resize_edges & WindowResizeEdge_Left) != 0) {
        const float clamped_delta = std::min(delta_x, state.resize_window_size_start.x - state.min_window_size.x);
        next_x = state.resize_window_pos_start.x + clamped_delta;
        next_width = state.resize_window_size_start.x - clamped_delta;
        moved_window = true;
    } else if ((state.resize_edges & WindowResizeEdge_Right) != 0) {
        next_width = std::max(state.min_window_size.x, state.resize_window_size_start.x + delta_x);
    }

    if ((state.resize_edges & WindowResizeEdge_Top) != 0) {
        const float clamped_delta = std::min(delta_y, state.resize_window_size_start.y - state.min_window_size.y);
        next_y = state.resize_window_pos_start.y + clamped_delta;
        next_height = state.resize_window_size_start.y - clamped_delta;
        moved_window = true;
    } else if ((state.resize_edges & WindowResizeEdge_Bottom) != 0) {
        next_height = std::max(state.min_window_size.y, state.resize_window_size_start.y + delta_y);
    }

    if (moved_window) {
        glfwSetWindowPos(window, static_cast<int>(next_x), static_cast<int>(next_y));
    }
    glfwSetWindowSize(window, static_cast<int>(next_width), static_cast<int>(next_height));
}

inline void update_title_bar_drag(GLFWwindow *window, const ImVec2 &drag_min, const ImVec2 &drag_max, WindowBarState &state) {
    const ImGuiIO &io = ImGui::GetIO();
    const bool can_drag = !state.resizing && ImGui::IsMouseHoveringRect(drag_min, drag_max) && !ImGui::IsAnyItemHovered();

    if (can_drag && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE) {
            glfwRestoreWindow(window);
        } else {
            glfwMaximizeWindow(window);
        }
        state.dragging = false;
        return;
    }

    if (can_drag && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        int window_x = 0;
        int window_y = 0;
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        glfwGetWindowPos(window, &window_x, &window_y);
        glfwGetCursorPos(window, &cursor_x, &cursor_y);

        state.dragging = true;
        state.drag_offset = ImVec2(static_cast<float>(cursor_x), static_cast<float>(cursor_y));
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        state.dragging = false;
    }

    if (state.dragging) {
        int window_x = 0;
        int window_y = 0;
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        glfwGetWindowPos(window, &window_x, &window_y);
        glfwGetCursorPos(window, &cursor_x, &cursor_y);

        const int next_x = window_x + static_cast<int>(cursor_x - state.drag_offset.x);
        const int next_y = window_y + static_cast<int>(cursor_y - state.drag_offset.y);
        glfwSetWindowPos(window, next_x, next_y);
    }
}

inline void draw_glfw_window_bar(GLFWwindow *window, const char *title, float height = 34.0F, WindowBarState *state = nullptr) {
    if (state == nullptr) {
        state = &default_window_bar_state();
    }

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const ImVec2 pos = viewport->Pos;
    const ImVec2 size(viewport->Size.x, height);
    update_window_resize(window, pos, viewport->Size, *state);

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, color::retina_dark::TitleBackground);
    ImGui::Begin("##qui_window_bar", nullptr, flags);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 min = ImGui::GetWindowPos();
    const ImVec2 max(min.x + ImGui::GetWindowWidth(), min.y + height);

    draw_list->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(color::retina_dark::TitleBackground));
    draw_list->AddLine(
        ImVec2(min.x, max.y - 1.0F),
        ImVec2(max.x, max.y - 1.0F),
        ImGui::ColorConvertFloat4ToU32(color::retina_dark::Border)
    );

    const ImVec2 accent_min(min.x + 10.0F, min.y + 10.0F);
    const ImVec2 accent_max(accent_min.x + 13.0F, accent_min.y + 13.0F);
    draw_list->AddRectFilled(accent_min, accent_max, ImGui::ColorConvertFloat4ToU32(color::retina_dark::AccentBlue), 2.0F);
    draw_list->AddRectFilled(ImVec2(accent_min.x + 4.0F, accent_min.y + 4.0F), accent_max, ImGui::ColorConvertFloat4ToU32(color::retina_dark::Highlight), 2.0F);

    if (ImFont *font = font_semi_bold()) {
        ImGui::PushFont(font);
    }
    draw_list->AddText(ImVec2(min.x + 34.0F, min.y + 8.0F), ImGui::ColorConvertFloat4ToU32(color::retina_dark::Text), title);
    if (font_semi_bold() != nullptr) {
        ImGui::PopFont();
    }

    const float button_width = 46.0F;
    const ImVec2 button_size(button_width, height);
    const float close_x = max.x - button_width;
    const float maximize_x = close_x - button_width;
    const float minimize_x = maximize_x - button_width;

    if (title_bar_button("minimize", "-", ImVec2(minimize_x, min.y), button_size)) {
        glfwIconifyWindow(window);
    }

    const bool maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE;
    if (title_bar_button("maximize", maximized ? "[]" : "[ ]", ImVec2(maximize_x, min.y), button_size)) {
        if (maximized) {
            glfwRestoreWindow(window);
        } else {
            glfwMaximizeWindow(window);
        }
    }

    if (title_bar_button("close", "X", ImVec2(close_x, min.y), button_size, true)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    update_title_bar_drag(window, ImVec2(min.x, min.y + 6.0F), ImVec2(minimize_x, max.y), *state);

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

} // namespace qui
