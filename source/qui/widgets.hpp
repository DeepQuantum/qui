#pragma once

#include <imgui.h>

#include <cstddef>

#include "qui/theme.hpp"
#include "qui/typography.hpp"

namespace qui {

inline void text_label(const char *text) {
    if (ImFont *font = font_semi_bold()) {
        ImGui::PushFont(font);
    }
    ImGui::PushStyleColor(ImGuiCol_Text, color::active_palette().TextDisabled);
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();
    if (font_semi_bold() != nullptr) {
        ImGui::PopFont();
    }
}

inline void section_label(const char *text) {
    if (ImFont *font = font_bold()) {
        ImGui::PushFont(font);
    }
    ImGui::PushStyleColor(ImGuiCol_Text, color::active_palette().Highlight);
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();
    if (font_bold() != nullptr) {
        ImGui::PopFont();
    }
}

inline void status_label(const char *text, const ImVec4 &color) {
    const ImVec2 padding(7.0F, 3.0F);
    const ImVec2 text_size = ImGui::CalcTextSize(text);
    const ImVec2 min = ImGui::GetCursorScreenPos();
    const ImVec2 max(min.x + text_size.x + padding.x * 2.0F, min.y + text_size.y + padding.y * 2.0F);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, 0.20F)), 3.0F);
    draw_list->AddRect(min, max, ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, 0.70F)), 3.0F);

    ImGui::SetCursorScreenPos(ImVec2(min.x + padding.x, min.y + padding.y));
    if (ImFont *font = font_bold()) {
        ImGui::PushFont(font);
    }
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(text);
    ImGui::PopStyleColor();
    if (font_bold() != nullptr) {
        ImGui::PopFont();
    }
    ImGui::SetCursorScreenPos(ImVec2(min.x, max.y + ImGui::GetStyle().ItemSpacing.y));
}

inline bool text_input(const char *label, char *buffer, std::size_t buffer_size, const char *hint = "", float width = -1.0F) {
    ImGui::BeginGroup();
    text_label(label);
    ImGui::SetNextItemWidth(width);

    ImGui::PushID(label);
    const bool changed = ImGui::InputTextWithHint("##input", hint, buffer, buffer_size);
    ImGui::PopID();

    ImGui::EndGroup();
    return changed;
}

inline bool begin_tabs(const char *id, ImGuiTabBarFlags flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll) {
    return ImGui::BeginTabBar(id, flags);
}

inline bool begin_tab(const char *label, bool *open = nullptr, ImGuiTabItemFlags flags = 0) {
    return ImGui::BeginTabItem(label, open, flags);
}

inline void end_tab() {
    ImGui::EndTabItem();
}

inline void end_tabs() {
    ImGui::EndTabBar();
}

inline bool begin_table(const char *id, int columns, const ImVec2 &size = ImVec2(0.0F, 0.0F)) {
    ImGuiTableFlags flags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_Hideable |
        ImGuiTableFlags_SizingStretchProp;

    if (size.y > 0.0F) {
        flags |= ImGuiTableFlags_ScrollY;
    }

    return ImGui::BeginTable(id, columns, flags, size);
}

inline void table_headers(const char *const *headers, int count) {
    for (int column = 0; column < count; ++column) {
        ImGui::TableSetupColumn(headers[column]);
    }
    ImGui::TableHeadersRow();
}

inline void end_table() {
    ImGui::EndTable();
}

inline void panel_background(const ImVec2 &min, const ImVec2 &max) {
    ImGui::GetWindowDrawList()->AddRectFilled(
        min,
        max,
        ImGui::ColorConvertFloat4ToU32(color::active_palette().Panel),
        ImGui::GetStyle().ChildRounding
    );
}

} // namespace qui
