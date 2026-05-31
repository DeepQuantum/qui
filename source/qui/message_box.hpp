#pragma once

#include <cfloat>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "qui/theme.hpp"
#include "qui/typography.hpp"

namespace qui {

inline constexpr int message_box_none = -1;
inline constexpr int message_box_dismissed = -2;

struct message_box_button {
    std::string label;
    ImVec4 color = color::retina_dark::Text;
    bool left = false;  // anchored to the bottom-left instead of bottom-right
    bool ghost = false; // borderless, text-only style (no filled background)
};

struct message_box {
    std::string popup_id = "##qui_message_box";
    std::string title = "Message";
    std::string message;
    std::vector<message_box_button> buttons;
    float width = 0.0F;        // 0 -> sensible default
    bool selectable = false;   // render the message as selectable / copyable text

    bool should_open = false;
    bool is_open = false;
    int result = message_box_none;

    void open() {
        should_open = true;
        result = message_box_none;
    }
};

inline void open_alert(message_box &box, std::string title, std::string message, std::string ok = "OK") {
    box.title = std::move(title);
    box.message = std::move(message);
    box.buttons.clear();
    box.buttons.push_back(message_box_button{std::move(ok)});
    box.open();
}

namespace detail {

inline float mb_button_width(const message_box_button &button) {
    return ImGui::CalcTextSize(button.label.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0F;
}

inline bool mb_button(const message_box_button &button) {
    int pushed = 1;
    ImGui::PushStyleColor(ImGuiCol_Text, button.color);
    if (button.ghost) {
        ImGui::PushStyleColor(ImGuiCol_Button, color::rgba(0x00, 0x00, 0x00, 0x00));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(button.color.x, button.color.y, button.color.z, 0.16F));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(button.color.x, button.color.y, button.color.z, 0.30F));
        pushed += 3;
    }
    const bool pressed = ImGui::Button(button.label.c_str());
    ImGui::PopStyleColor(pushed);
    return pressed;
}

} // namespace detail

// Renders the message box if it is open. Returns the index of the pressed
// button on the frame it is clicked, message_box_dismissed if it was closed
// without a choice (e.g. Escape), or message_box_none otherwise.
inline int draw_message_box(message_box &box) {
    if (box.should_open) {
        ImGui::OpenPopup(box.popup_id.c_str());
        box.should_open = false;
        box.is_open = true;
    }

    const ImGuiStyle &style = ImGui::GetStyle();
    const float pad = 14.0F;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x + viewport->Size.x * 0.5F, viewport->Pos.y + viewport->Size.y * 0.5F),
        ImGuiCond_Appearing,
        ImVec2(0.5F, 0.5F)
    );

    float left_total = 0.0F;
    float right_total = 0.0F;
    int left_count = 0;
    int right_count = 0;
    for (const message_box_button &button : box.buttons) {
        if (button.left) {
            left_total += detail::mb_button_width(button) + style.ItemSpacing.x;
            ++left_count;
        } else {
            right_total += detail::mb_button_width(button) + style.ItemSpacing.x;
            ++right_count;
        }
    }
    if (left_count > 0) {
        left_total -= style.ItemSpacing.x;
    }
    if (right_count > 0) {
        right_total -= style.ItemSpacing.x;
    }

    float width = box.width > 0.0F ? box.width : ImGui::GetFontSize() * 19.0F;
    const float buttons_width = pad + left_total + (left_count > 0 && right_count > 0 ? style.ItemSpacing.x * 2.0F : 0.0F) + right_total + pad;
    if (buttons_width > width) {
        width = buttons_width;
    }
    ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0.0F), ImVec2(width, FLT_MAX));

    int result = message_box_none;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::BeginPopupModal(box.popup_id.c_str(), nullptr, flags)) {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        const ImVec2 win_pos = ImGui::GetWindowPos();
        const float win_width = ImGui::GetWindowWidth();
        const float header_h = ImGui::GetFontSize() + 18.0F;

        draw_list->AddRectFilled(
            win_pos,
            ImVec2(win_pos.x + win_width, win_pos.y + header_h),
            ImGui::ColorConvertFloat4ToU32(color::retina_dark::TitleBackground),
            style.PopupRounding,
            ImDrawFlags_RoundCornersTop
        );
        draw_list->AddLine(
            ImVec2(win_pos.x, win_pos.y + header_h),
            ImVec2(win_pos.x + win_width, win_pos.y + header_h),
            ImGui::ColorConvertFloat4ToU32(color::retina_dark::Border)
        );

        const ImVec2 logo_min(win_pos.x + pad, win_pos.y + (header_h - 13.0F) * 0.5F);
        const ImVec2 logo_max(logo_min.x + 13.0F, logo_min.y + 13.0F);
        draw_list->AddRectFilled(logo_min, logo_max, ImGui::ColorConvertFloat4ToU32(color::retina_dark::AccentBlue), 2.0F);
        draw_list->AddRectFilled(ImVec2(logo_min.x + 4.0F, logo_min.y + 4.0F), logo_max, ImGui::ColorConvertFloat4ToU32(color::retina_dark::Highlight), 2.0F);

        if (ImFont *font = font_semi_bold()) {
            ImGui::PushFont(font);
        }
        const float title_h = ImGui::GetFontSize();
        draw_list->AddText(
            ImVec2(logo_max.x + 8.0F, win_pos.y + (header_h - title_h) * 0.5F),
            ImGui::ColorConvertFloat4ToU32(color::retina_dark::Text),
            box.title.c_str()
        );
        if (font_semi_bold() != nullptr) {
            ImGui::PopFont();
        }

        ImGui::SetCursorPos(ImVec2(pad, header_h + pad));
        if (ImFont *font = font_medium()) {
            ImGui::PushFont(font);
        }
        if (box.selectable) {
            const float wrap_w = win_width - pad * 2.0F;
            // Match the multiline input's internal wrap width (it reserves frame
            // padding and a scrollbar slot) so the measured height never clips.
            const float inner_wrap = wrap_w - style.ScrollbarSize - style.FramePadding.x * 2.0F;
            const float text_h = ImGui::CalcTextSize(box.message.c_str(), nullptr, false, inner_wrap > 1.0F ? inner_wrap : wrap_w).y;
            ImGui::PushStyleColor(ImGuiCol_FrameBg, color::rgba(0x00, 0x00, 0x00, 0x00));
            ImGui::PushStyleColor(ImGuiCol_Text, color::retina_dark::Text);
            ImGui::InputTextMultiline(
                "##message",
                &box.message,
                ImVec2(wrap_w, text_h + style.FramePadding.y * 2.0F + 4.0F),
                ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap
            );
            ImGui::PopStyleColor(2);
        } else {
            ImGui::PushTextWrapPos(win_width - pad);
            ImGui::TextUnformatted(box.message.c_str());
            ImGui::PopTextWrapPos();
        }
        if (font_medium() != nullptr) {
            ImGui::PopFont();
        }

        const float buttons_y = ImGui::GetCursorPosY() + pad;
        const float content_right = win_width - pad;

        bool first = true;
        for (std::size_t i = 0; i < box.buttons.size(); ++i) {
            if (!box.buttons[i].left) {
                continue;
            }
            ImGui::SetCursorPos(ImVec2(first ? pad : ImGui::GetCursorPosX(), buttons_y));
            if (!first) {
                ImGui::SameLine(0.0F, style.ItemSpacing.x);
            }
            ImGui::PushID(static_cast<int>(i));
            if (detail::mb_button(box.buttons[i])) {
                result = static_cast<int>(i);
            }
            ImGui::PopID();
            first = false;
        }

        if (right_count > 0) {
            ImGui::SetCursorPos(ImVec2(content_right - right_total, buttons_y));
            bool first_right = true;
            for (std::size_t i = 0; i < box.buttons.size(); ++i) {
                if (box.buttons[i].left) {
                    continue;
                }
                if (!first_right) {
                    ImGui::SameLine(0.0F, style.ItemSpacing.x);
                }
                ImGui::PushID(static_cast<int>(i));
                if (detail::mb_button(box.buttons[i])) {
                    result = static_cast<int>(i);
                }
                ImGui::PopID();
                first_right = false;
            }
        }

        ImGui::SetCursorPos(ImVec2(pad, buttons_y + ImGui::GetFrameHeight()));
        ImGui::Dummy(ImVec2(0.0F, pad));

        if (result != message_box_none) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    } else if (box.is_open) {
        result = message_box_dismissed;
    }

    ImGui::PopStyleVar();

    if (result != message_box_none) {
        box.is_open = false;
        box.result = result;
    }
    return result;
}

} // namespace qui
