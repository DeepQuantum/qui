#pragma once

#include <imgui.h>

namespace qui {

namespace color {

inline constexpr ImVec4 rgba(int r, int g, int b, int a = 0xFF) {
    return {
        static_cast<float>(r) / 255.0F,
        static_cast<float>(g) / 255.0F,
        static_cast<float>(b) / 255.0F,
        static_cast<float>(a) / 255.0F,
    };
}

namespace retina_dark {

inline constexpr ImVec4 Border                  = rgba(0x6D, 0x6D, 0x7F, 0x7F);
inline constexpr ImVec4 Button                  = rgba(0x42, 0x96, 0xF9, 0x66);
inline constexpr ImVec4 ButtonActive            = rgba(0x0F, 0x87, 0xF9);
inline constexpr ImVec4 ButtonHovered           = rgba(0x42, 0x96, 0xF9);
inline constexpr ImVec4 FrameBackground         = rgba(0x28, 0x49, 0x7A, 0x89);
inline constexpr ImVec4 FrameBackgroundActive   = rgba(0x42, 0x96, 0xF9, 0xAA);
inline constexpr ImVec4 FrameBackgroundHovered  = rgba(0x42, 0x96, 0xF9, 0x66);
inline constexpr ImVec4 Header                  = rgba(0x42, 0x96, 0xF9, 0x4F);
inline constexpr ImVec4 HeaderActive            = rgba(0x42, 0x96, 0xF9);
inline constexpr ImVec4 HeaderHovered           = rgba(0x42, 0x96, 0xF9, 0xCC);
inline constexpr ImVec4 MenuBarBackground       = rgba(0x1B, 0x3F, 0x6E);
inline constexpr ImVec4 PopupBackground         = rgba(0x14, 0x14, 0x14);
inline constexpr ImVec4 ScrollbarBackground     = rgba(0x05, 0x05, 0x05, 0x87);
inline constexpr ImVec4 ScrollbarGrab           = rgba(0x4F, 0x4F, 0x4F);
inline constexpr ImVec4 ScrollbarGrabActive     = rgba(0x82, 0x82, 0x82);
inline constexpr ImVec4 ScrollbarGrabHovered    = rgba(0x68, 0x68, 0x68);
inline constexpr ImVec4 SeparatorActive         = rgba(0x19, 0x66, 0xBF);
inline constexpr ImVec4 SeparatorHovered        = rgba(0x19, 0x66, 0xBF, 0xC6);
inline constexpr ImVec4 SliderGrab              = rgba(0x3D, 0x84, 0xE0);
inline constexpr ImVec4 SliderGrabActive        = rgba(0x42, 0x96, 0xF9);
inline constexpr ImVec4 Tab                     = rgba(0x2D, 0x59, 0x93, 0xDB);
inline constexpr ImVec4 TabActive               = rgba(0x32, 0x68, 0xAD);
inline constexpr ImVec4 TabHovered              = rgba(0x42, 0x96, 0xF9, 0xCC);
inline constexpr ImVec4 TabUnfocused            = rgba(0x11, 0x1A, 0x25, 0xF7);
inline constexpr ImVec4 TabUnfocusedActive      = rgba(0x22, 0x42, 0x6C);
inline constexpr ImVec4 TableBorderLight        = rgba(0x3A, 0x3A, 0x3F);
inline constexpr ImVec4 TableBorderStrong       = rgba(0x4F, 0x4F, 0x59);
inline constexpr ImVec4 TableHeaderBackground   = rgba(0x30, 0x30, 0x33);
inline constexpr ImVec4 TableRowBackground      = rgba(0x00, 0x00, 0x00, 0x00);
inline constexpr ImVec4 TableRowBackgroundAlt   = rgba(0xFF, 0xFF, 0xFF, 0x0F);
inline constexpr ImVec4 Text                    = rgba(0xFF, 0xFF, 0xFF);
inline constexpr ImVec4 TextDisabled            = rgba(0x7F, 0x7F, 0x7F);
inline constexpr ImVec4 TextSelectedBackground  = rgba(0x42, 0x96, 0xF9, 0x59);
inline constexpr ImVec4 TitleBackground         = rgba(0x16, 0x35, 0x5E);
inline constexpr ImVec4 WindowBackground        = rgba(0x0F, 0x0F, 0x0F);

inline constexpr ImVec4 AccentBlue              = rgba(0x06, 0x53, 0x9B);
inline constexpr ImVec4 AccentGreen             = rgba(0x38, 0x8B, 0x42);
inline constexpr ImVec4 AccentPurple            = rgba(0x67, 0x2A, 0x78);
inline constexpr ImVec4 AccentRed               = rgba(0xE7, 0x4C, 0x3C);
inline constexpr ImVec4 AccentYellow            = rgba(0xF1, 0xC4, 0x0F);
inline constexpr ImVec4 Highlight               = rgba(0x4D, 0xC6, 0x9B);
inline constexpr ImVec4 Panel                   = rgba(0x18, 0x18, 0x18);
inline constexpr ImVec4 PanelRaised             = rgba(0x23, 0x23, 0x23);

} // namespace retina_dark

// A runtime, overridable copy of the named theme colors. Widgets read from the
// active palette instead of the compile-time retina_dark constants so the host
// app can swap the entire UI theme at runtime. Defaults to retina_dark.
struct palette {
    ImVec4 Border, Button, ButtonActive, ButtonHovered;
    ImVec4 FrameBackground, FrameBackgroundActive, FrameBackgroundHovered;
    ImVec4 Header, HeaderActive, HeaderHovered;
    ImVec4 MenuBarBackground, PopupBackground;
    ImVec4 ScrollbarBackground, ScrollbarGrab, ScrollbarGrabActive, ScrollbarGrabHovered;
    ImVec4 SeparatorActive, SeparatorHovered;
    ImVec4 SliderGrab, SliderGrabActive;
    ImVec4 Tab, TabActive, TabHovered, TabUnfocused, TabUnfocusedActive;
    ImVec4 TableBorderLight, TableBorderStrong, TableHeaderBackground, TableRowBackground, TableRowBackgroundAlt;
    ImVec4 Text, TextDisabled, TextSelectedBackground;
    ImVec4 TitleBackground, WindowBackground;
    ImVec4 AccentBlue, AccentGreen, AccentPurple, AccentRed, AccentYellow;
    ImVec4 Highlight, Panel, PanelRaised;
};

inline palette make_retina_dark_palette() {
    using namespace retina_dark;
    palette p;
    p.Border = Border;
    p.Button = Button;
    p.ButtonActive = ButtonActive;
    p.ButtonHovered = ButtonHovered;
    p.FrameBackground = FrameBackground;
    p.FrameBackgroundActive = FrameBackgroundActive;
    p.FrameBackgroundHovered = FrameBackgroundHovered;
    p.Header = Header;
    p.HeaderActive = HeaderActive;
    p.HeaderHovered = HeaderHovered;
    p.MenuBarBackground = MenuBarBackground;
    p.PopupBackground = PopupBackground;
    p.ScrollbarBackground = ScrollbarBackground;
    p.ScrollbarGrab = ScrollbarGrab;
    p.ScrollbarGrabActive = ScrollbarGrabActive;
    p.ScrollbarGrabHovered = ScrollbarGrabHovered;
    p.SeparatorActive = SeparatorActive;
    p.SeparatorHovered = SeparatorHovered;
    p.SliderGrab = SliderGrab;
    p.SliderGrabActive = SliderGrabActive;
    p.Tab = Tab;
    p.TabActive = TabActive;
    p.TabHovered = TabHovered;
    p.TabUnfocused = TabUnfocused;
    p.TabUnfocusedActive = TabUnfocusedActive;
    p.TableBorderLight = TableBorderLight;
    p.TableBorderStrong = TableBorderStrong;
    p.TableHeaderBackground = TableHeaderBackground;
    p.TableRowBackground = TableRowBackground;
    p.TableRowBackgroundAlt = TableRowBackgroundAlt;
    p.Text = Text;
    p.TextDisabled = TextDisabled;
    p.TextSelectedBackground = TextSelectedBackground;
    p.TitleBackground = TitleBackground;
    p.WindowBackground = WindowBackground;
    p.AccentBlue = AccentBlue;
    p.AccentGreen = AccentGreen;
    p.AccentPurple = AccentPurple;
    p.AccentRed = AccentRed;
    p.AccentYellow = AccentYellow;
    p.Highlight = Highlight;
    p.Panel = Panel;
    p.PanelRaised = PanelRaised;
    return p;
}

inline palette &active_palette() {
    static palette p = make_retina_dark_palette();
    return p;
}

} // namespace color

inline void apply_palette_colors(const color::palette &P, ImGuiStyle &style = ImGui::GetStyle()) {
    auto *colors = style.Colors;
    colors[ImGuiCol_Text] = P.Text;
    colors[ImGuiCol_TextDisabled] = P.TextDisabled;
    colors[ImGuiCol_WindowBg] = P.WindowBackground;
    colors[ImGuiCol_ChildBg] = color::rgba(0x00, 0x00, 0x00, 0x00);
    colors[ImGuiCol_PopupBg] = P.PopupBackground;
    colors[ImGuiCol_Border] = P.Border;
    colors[ImGuiCol_BorderShadow] = color::rgba(0x00, 0x00, 0x00, 0x00);
    colors[ImGuiCol_FrameBg] = P.FrameBackground;
    colors[ImGuiCol_FrameBgHovered] = P.FrameBackgroundHovered;
    colors[ImGuiCol_FrameBgActive] = P.FrameBackgroundActive;
    colors[ImGuiCol_TitleBg] = P.TitleBackground;
    colors[ImGuiCol_TitleBgActive] = P.TitleBackground;
    colors[ImGuiCol_TitleBgCollapsed] = P.TitleBackground;
    colors[ImGuiCol_MenuBarBg] = P.MenuBarBackground;
    colors[ImGuiCol_ScrollbarBg] = P.ScrollbarBackground;
    colors[ImGuiCol_ScrollbarGrab] = P.ScrollbarGrab;
    colors[ImGuiCol_ScrollbarGrabHovered] = P.ScrollbarGrabHovered;
    colors[ImGuiCol_ScrollbarGrabActive] = P.ScrollbarGrabActive;
    colors[ImGuiCol_CheckMark] = P.ButtonHovered;
    colors[ImGuiCol_SliderGrab] = P.SliderGrab;
    colors[ImGuiCol_SliderGrabActive] = P.SliderGrabActive;
    colors[ImGuiCol_Button] = P.Button;
    colors[ImGuiCol_ButtonHovered] = P.ButtonHovered;
    colors[ImGuiCol_ButtonActive] = P.ButtonActive;
    colors[ImGuiCol_Header] = P.Header;
    colors[ImGuiCol_HeaderHovered] = P.HeaderHovered;
    colors[ImGuiCol_HeaderActive] = P.HeaderActive;
    colors[ImGuiCol_Separator] = P.Border;
    colors[ImGuiCol_SeparatorHovered] = P.SeparatorHovered;
    colors[ImGuiCol_SeparatorActive] = P.SeparatorActive;
    colors[ImGuiCol_ResizeGrip] = color::rgba(0x42, 0x96, 0xF9, 0x33);
    colors[ImGuiCol_ResizeGripHovered] = color::rgba(0x42, 0x96, 0xF9, 0xAA);
    colors[ImGuiCol_ResizeGripActive] = color::rgba(0x42, 0x96, 0xF9, 0xF2);
    colors[ImGuiCol_Tab] = P.Tab;
    colors[ImGuiCol_TabHovered] = P.TabHovered;
    colors[ImGuiCol_TabActive] = P.TabActive;
    colors[ImGuiCol_TabUnfocused] = P.TabUnfocused;
    colors[ImGuiCol_TabUnfocusedActive] = P.TabUnfocusedActive;
    colors[ImGuiCol_TableHeaderBg] = P.TableHeaderBackground;
    colors[ImGuiCol_TableBorderStrong] = P.TableBorderStrong;
    colors[ImGuiCol_TableBorderLight] = P.TableBorderLight;
    colors[ImGuiCol_TableRowBg] = P.TableRowBackground;
    colors[ImGuiCol_TableRowBgAlt] = P.TableRowBackgroundAlt;
    colors[ImGuiCol_TextSelectedBg] = P.TextSelectedBackground;
    colors[ImGuiCol_DragDropTarget] = color::rgba(0xFF, 0xFF, 0x00, 0xE5);
    colors[ImGuiCol_NavHighlight] = P.ButtonHovered;
    colors[ImGuiCol_NavWindowingHighlight] = color::rgba(0xFF, 0xFF, 0xFF, 0xB2);
    colors[ImGuiCol_NavWindowingDimBg] = color::rgba(0xCC, 0xCC, 0xCC, 0x33);
    colors[ImGuiCol_ModalWindowDimBg] = color::rgba(0xCC, 0xCC, 0xCC, 0x59);

#ifdef ImGuiCol_DockingPreview
    colors[ImGuiCol_DockingPreview] = color::rgba(0x42, 0x96, 0xF9, 0xB2);
    colors[ImGuiCol_DockingEmptyBg] = color::rgba(0x0F, 0x0F, 0x0F, 0xEF);
#endif
}

inline void apply_retina_dark_theme(ImGuiStyle &style = ImGui::GetStyle()) {
    ImGui::StyleColorsDark(&style);

    style.WindowPadding = ImVec2(8.0F, 8.0F);
    style.FramePadding = ImVec2(7.0F, 4.0F);
    style.CellPadding = ImVec2(6.0F, 3.0F);
    style.ItemSpacing = ImVec2(8.0F, 6.0F);
    style.ItemInnerSpacing = ImVec2(6.0F, 4.0F);
    style.ScrollbarSize = 14.0F;
    style.GrabMinSize = 10.0F;
    style.WindowBorderSize = 1.0F;
    style.ChildBorderSize = 1.0F;
    style.PopupBorderSize = 1.0F;
    style.FrameBorderSize = 0.0F;
    style.TabBorderSize = 0.0F;
    style.WindowRounding = 4.0F;
    style.ChildRounding = 3.0F;
    style.FrameRounding = 3.0F;
    style.PopupRounding = 3.0F;
    style.ScrollbarRounding = 3.0F;
    style.GrabRounding = 3.0F;
    style.TabRounding = 3.0F;

    apply_palette_colors(color::active_palette(), style);
}

} // namespace qui
