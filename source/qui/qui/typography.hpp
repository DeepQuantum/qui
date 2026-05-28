#pragma once

#include <algorithm>

#include <imgui.h>

namespace qui {

struct FontSet {
    ImFont *medium = nullptr;
    ImFont *semi_bold = nullptr;
    ImFont *bold = nullptr;
    float dpi_scale = 1.0F;
};

inline FontSet &default_fonts() {
    static FontSet fonts;
    return fonts;
}

inline const FontSet *&active_fonts() {
    static const FontSet *fonts = nullptr;
    return fonts;
}

inline void set_active_fonts(const FontSet &fonts) {
    active_fonts() = &fonts;
}

inline ImFont *font_medium() {
    const FontSet *fonts = active_fonts();
    return fonts != nullptr ? fonts->medium : nullptr;
}

inline ImFont *font_semi_bold() {
    const FontSet *fonts = active_fonts();
    return fonts != nullptr ? fonts->semi_bold : nullptr;
}

inline ImFont *font_bold() {
    const FontSet *fonts = active_fonts();
    return fonts != nullptr ? fonts->bold : nullptr;
}

inline ImFontConfig font_config(float dpi_scale, float rasterizer_multiply = 1.0F) {
    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 2;
    config.PixelSnapH = false;
    config.RasterizerMultiply = rasterizer_multiply;
    config.RasterizerDensity = std::max(dpi_scale, 1.0F);
    return config;
}

inline ImFont *load_font_or_default(const char *path, float size_pixels, const ImFontConfig &config) {
    ImGuiIO &io = ImGui::GetIO();

    if (path != nullptr && path[0] != '\0') {
        if (ImFont *font = io.Fonts->AddFontFromFileTTF(path, size_pixels, &config)) {
            return font;
        }
    }

    return io.Fonts->AddFontDefault(&config);
}

inline FontSet &load_default_fonts(float dpi_scale = 1.0F, float size = 17.0F) {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();

    const float density = std::max(dpi_scale, 1.0F);

    FontSet &fonts = default_fonts();
    fonts.dpi_scale = density;

#ifdef QUI_BASE_FONT_DIR
    const char *medium_font_path = QUI_BASE_FONT_DIR "/GeistMono-Medium.ttf";
    const char *semi_bold_font_path = QUI_BASE_FONT_DIR "/GeistMono-SemiBold.ttf";
    const char *bold_font_path = QUI_BASE_FONT_DIR "/GeistMono-Bold.ttf";
#else
    const char *medium_font_path = "";
    const char *semi_bold_font_path = "";
    const char *bold_font_path = "";
#endif

    fonts.medium = load_font_or_default(medium_font_path, size, font_config(density, 1.06F));
    fonts.semi_bold = load_font_or_default(semi_bold_font_path, size, font_config(density, 1.04F));
    fonts.bold = load_font_or_default(bold_font_path, size, font_config(density, 1.02F));

    io.FontDefault = fonts.medium;
    set_active_fonts(fonts);
    return fonts;
}

inline void apply_dpi_scale(float dpi_scale, ImGuiStyle &style = ImGui::GetStyle()) {
    const float density = std::max(dpi_scale, 1.0F);

    if (density > 1.0F) {
        style.ScaleAllSizes(density);
    }

    style.FontScaleDpi = 1.0F;
}

} // namespace qui
