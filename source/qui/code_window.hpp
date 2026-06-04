#pragma once

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <string>
#include <span>
#include <string_view>
#include <vector>

#include <imgui.h>

#include "qui/theme.hpp"
#include "qui/typography.hpp"

namespace qui {
namespace code {

struct theme {
    ImU32 background;
    ImU32 gutter_background;
    ImU32 gutter_text;
    ImU32 current_line;
    ImU32 text;
};

inline ImU32 to_u32(const ImVec4 &c) {
    return ImGui::ColorConvertFloat4ToU32(c);
}

inline const theme &default_theme() {
    static const theme value = [] {
        using namespace qui::color;
        theme t{};
        t.background = to_u32(active_palette().Panel);
        t.gutter_background = to_u32(active_palette().WindowBackground);
        t.gutter_text = to_u32(active_palette().TextDisabled);
        t.current_line = to_u32(rgba(0xFF, 0xFF, 0xFF, 0x0D));
        t.text = to_u32(active_palette().Text);
        return t;
    }();
    return value;
}

// Reports the spans of one matched pattern within a single line by invoking
// `claim(begin, end)` for every (non-overlapping) match.
using span_sink = std::function<void(std::size_t begin, std::size_t end)>;
using matcher = std::function<void(std::string_view line, const span_sink &claim)>;

// A syntax-highlighting rule: one pattern matcher plus the color its matches
// should be drawn in. The widget applies a collection of these in order;
// earlier rules win over later ones for overlapping characters.
struct rule {
    matcher match;
    ImU32 color;
};

inline void tokenize_line(std::string_view line, std::span<const rule> rules, const theme &th, std::vector<ImU32> &colors) {
    colors.assign(line.size(), th.text);
    if (line.empty()) {
        return;
    }
    std::vector<unsigned char> claimed(line.size(), 0);
    for (const rule &r : rules) {
        if (!r.match) {
            continue;
        }
        r.match(line, [&](std::size_t begin, std::size_t end) {
            end = std::min(end, line.size());
            for (std::size_t i = begin; i < end; ++i) {
                if (claimed[i] == 0) {
                    claimed[i] = 1;
                    colors[i] = r.color;
                }
            }
        });
    }
}

inline void split_lines(std::string_view text, std::vector<std::string_view> &lines) {
    lines.clear();
    std::size_t start = 0;
    for (std::size_t i = 0; i <= text.size(); ++i) {
        if (i == text.size() || text[i] == '\n') {
            std::size_t end = i;
            if (end > start && text[end - 1] == '\r') {
                --end;
            }
            lines.push_back(text.substr(start, end - start));
            start = i + 1;
        }
    }
    if (lines.empty()) {
        lines.emplace_back();
    }
}

inline void code_window(const char *id, std::string_view text, std::span<const rule> rules, const ImVec2 &size = ImVec2(0.0F, 0.0F), const theme &th = default_theme()) {
    std::vector<std::string_view> lines;
    split_lines(text, lines);

    std::size_t max_chars = 0;
    for (const std::string_view &line : lines) {
        max_chars = std::max(max_chars, line.size());
    }

    int digits = 1;
    for (std::size_t n = lines.size(); n >= 10; n /= 10) {
        ++digits;
    }

    ImFont *mono = qui::font_medium();
    if (mono != nullptr) {
        ImGui::PushFont(mono);
    }
    const float char_w = ImGui::CalcTextSize("0").x;
    const float line_h = ImGui::GetTextLineHeight() + 2.0F;
    if (mono != nullptr) {
        ImGui::PopFont();
    }

    const float pad = char_w * 0.75F;
    const float gutter_w = pad + static_cast<float>(digits) * char_w + pad;
    const float content_w = gutter_w + static_cast<float>(max_chars) * char_w + char_w;

    // When no explicit height is requested, fit the child exactly to the lines.
    const ImGuiStyle &style = ImGui::GetStyle();
    ImVec2 effective_size = size;
    if (effective_size.y <= 0.0F) {
        const float view_w = effective_size.x > 0.0F ? effective_size.x : ImGui::GetContentRegionAvail().x;
        effective_size.y = static_cast<float>(lines.size()) * line_h + style.ChildBorderSize * 2.0F;
        if (content_w > view_w) {
            effective_size.y += style.ScrollbarSize;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, th.background);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    ImGui::BeginChild(id, effective_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

    if (mono != nullptr) {
        ImGui::PushFont(mono);
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    const ImU32 divider = to_u32(qui::color::active_palette().Border);
    const float mouse_y = ImGui::GetMousePos().y;
    const bool window_hovered = ImGui::IsWindowHovered();

    std::vector<ImU32> colors;
    char number_buffer[16];

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0F, 0.0F));
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(lines.size()), line_h);
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
            const ImVec2 pos = ImGui::GetCursorScreenPos();
            const std::string_view line = lines[static_cast<std::size_t>(row)];

            draw_list->AddRectFilled(pos, ImVec2(pos.x + gutter_w, pos.y + line_h), th.gutter_background);
            if (window_hovered && mouse_y >= pos.y && mouse_y < pos.y + line_h) {
                draw_list->AddRectFilled(ImVec2(pos.x + gutter_w, pos.y), ImVec2(pos.x + content_w, pos.y + line_h), th.current_line);
            }
            draw_list->AddLine(ImVec2(pos.x + gutter_w, pos.y), ImVec2(pos.x + gutter_w, pos.y + line_h), divider);

            std::snprintf(number_buffer, sizeof(number_buffer), "%d", row + 1);
            const float number_w = ImGui::CalcTextSize(number_buffer).x;
            draw_list->AddText(ImVec2(pos.x + gutter_w - pad - number_w, pos.y + 1.0F), th.gutter_text, number_buffer);

            tokenize_line(line, rules, th, colors);
            float x = pos.x + gutter_w + pad;
            std::size_t i = 0;
            while (i < line.size()) {
                std::size_t j = i + 1;
                while (j < line.size() && colors[j] == colors[i]) {
                    ++j;
                }
                const char *begin = line.data() + i;
                const char *end = line.data() + j;
                draw_list->AddText(ImVec2(x, pos.y + 1.0F), colors[i], begin, end);
                x += ImGui::CalcTextSize(begin, end).x;
                i = j;
            }

            ImGui::Dummy(ImVec2(content_w, line_h));
        }
    }
    clipper.End();
    ImGui::PopStyleVar();

    if (mono != nullptr) {
        ImGui::PopFont();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// A pre-colored run of text. Unlike the rule-based code_window, the caller has
// already decided each segment's color (e.g. from an AST walk), so no
// per-character tokenization happens here. Segments may contain newlines.
struct colored_span {
    ImU32 color;
    std::string_view text;
};

struct hovered_token {
    bool hovered = false;
    ImU32 color = 0;
    std::string text;
    std::string line_text;
    std::size_t line = 0;
    std::size_t column = 0;
    ImVec2 screen_pos = ImVec2(0.0F, 0.0F);
};

inline bool is_token_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

inline void code_window_colored(
    const char *id,
    std::span<const colored_span> spans,
    const ImVec2 &size = ImVec2(0.0F, 0.0F),
    const theme &th = default_theme(),
    hovered_token *hovered = nullptr
) {
    struct run {
        ImU32 color;
        std::string_view text;
    };

    if (hovered != nullptr) {
        *hovered = {};
    }

    std::vector<std::vector<run>> lines;
    lines.emplace_back();
    std::size_t max_chars = 0;
    std::size_t cur_chars = 0;

    auto push_piece = [&](ImU32 color, std::string_view piece) {
        if (!piece.empty() && piece.back() == '\r') {
            piece.remove_suffix(1);
        }
        if (!piece.empty()) {
            lines.back().push_back({color, piece});
            cur_chars += piece.size();
        }
    };

    for (const colored_span &span : spans) {
        const std::string_view text = span.text;
        std::size_t start = 0;
        for (std::size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '\n') {
                push_piece(span.color, text.substr(start, i - start));
                max_chars = std::max(max_chars, cur_chars);
                cur_chars = 0;
                lines.emplace_back();
                start = i + 1;
            }
        }
        push_piece(span.color, text.substr(start));
    }
    max_chars = std::max(max_chars, cur_chars);

    int digits = 1;
    for (std::size_t n = lines.size(); n >= 10; n /= 10) {
        ++digits;
    }

    ImFont *mono = qui::font_medium();
    if (mono != nullptr) {
        ImGui::PushFont(mono);
    }
    const float char_w = ImGui::CalcTextSize("0").x;
    const float line_h = ImGui::GetTextLineHeight() + 2.0F;
    if (mono != nullptr) {
        ImGui::PopFont();
    }

    const float pad = char_w * 0.75F;
    const float gutter_w = pad + static_cast<float>(digits) * char_w + pad;
    const float content_w = gutter_w + static_cast<float>(max_chars) * char_w + char_w;

    const ImGuiStyle &style = ImGui::GetStyle();
    ImVec2 effective_size = size;
    if (effective_size.y <= 0.0F) {
        const float view_w = effective_size.x > 0.0F ? effective_size.x : ImGui::GetContentRegionAvail().x;
        effective_size.y = static_cast<float>(lines.size()) * line_h + style.ChildBorderSize * 2.0F;
        if (content_w > view_w) {
            effective_size.y += style.ScrollbarSize;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, th.background);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    ImGui::BeginChild(id, effective_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

    if (mono != nullptr) {
        ImGui::PushFont(mono);
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImU32 divider = to_u32(qui::color::active_palette().Border);
    const float mouse_y = ImGui::GetMousePos().y;
    const float mouse_x = ImGui::GetMousePos().x;
    const bool window_hovered = ImGui::IsWindowHovered();
    char number_buffer[16];

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0F, 0.0F));
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(lines.size()), line_h);
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
            const ImVec2 pos = ImGui::GetCursorScreenPos();

            draw_list->AddRectFilled(pos, ImVec2(pos.x + gutter_w, pos.y + line_h), th.gutter_background);
            if (window_hovered && mouse_y >= pos.y && mouse_y < pos.y + line_h) {
                draw_list->AddRectFilled(ImVec2(pos.x + gutter_w, pos.y), ImVec2(pos.x + content_w, pos.y + line_h), th.current_line);
            }
            draw_list->AddLine(ImVec2(pos.x + gutter_w, pos.y), ImVec2(pos.x + gutter_w, pos.y + line_h), divider);

            std::snprintf(number_buffer, sizeof(number_buffer), "%d", row + 1);
            const float number_w = ImGui::CalcTextSize(number_buffer).x;
            draw_list->AddText(ImVec2(pos.x + gutter_w - pad - number_w, pos.y + 1.0F), th.gutter_text, number_buffer);

            float x = pos.x + gutter_w + pad;
            std::size_t column = 0;
            for (const run &r : lines[static_cast<std::size_t>(row)]) {
                const char *begin = r.text.data();
                const char *end = begin + r.text.size();
                const float run_w = ImGui::CalcTextSize(begin, end).x;
                if (hovered != nullptr && !hovered->hovered && window_hovered &&
                    mouse_y >= pos.y && mouse_y < pos.y + line_h &&
                    mouse_x >= x && mouse_x < x + run_w && !r.text.empty()) {
                    std::size_t char_index = static_cast<std::size_t>((mouse_x - x) / std::max(char_w, 1.0F));
                    char_index = std::min(char_index, r.text.size() - 1);
                    if (is_token_char(r.text[char_index])) {
                        std::size_t token_begin = char_index;
                        while (token_begin > 0 && is_token_char(r.text[token_begin - 1])) {
                            --token_begin;
                        }
                        std::size_t token_end = char_index + 1;
                        while (token_end < r.text.size() && is_token_char(r.text[token_end])) {
                            ++token_end;
                        }
                        hovered->hovered = true;
                        hovered->color = r.color;
                        hovered->text = std::string(r.text.substr(token_begin, token_end - token_begin));
                        hovered->line = static_cast<std::size_t>(row);
                        hovered->column = column + token_begin;
                        hovered->screen_pos = ImVec2(x + static_cast<float>(token_begin) * char_w, pos.y);
                        for (const run &line_run : lines[static_cast<std::size_t>(row)]) {
                            hovered->line_text.append(line_run.text);
                        }
                    }
                }
                draw_list->AddText(ImVec2(x, pos.y + 1.0F), r.color, begin, end);
                x += run_w;
                column += r.text.size();
            }

            ImGui::Dummy(ImVec2(content_w, line_h));
        }
    }
    clipper.End();
    ImGui::PopStyleVar();

    if (mono != nullptr) {
        ImGui::PopFont();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

} // namespace code
} // namespace qui
