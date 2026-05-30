#pragma once

#include <cstddef>
#include <string_view>

#include <ctre.hpp>

#include "qui/code_window.hpp"

namespace qui {
namespace code {

// Builds a code_window rule from a compile-time CTRE pattern. The pattern is
// matched against each line and every match is reported to the widget; the
// caller supplies the color and the order (precedence) of the rules.
template <ctll::fixed_string Pattern>
inline rule ctre_rule(ImU32 color) {
    return rule{
        [](std::string_view line, const span_sink &claim) {
            for (auto match : ctre::range<Pattern>(line)) {
                const std::string_view span = match.to_view();
                const std::size_t begin = static_cast<std::size_t>(span.data() - line.data());
                claim(begin, begin + span.size());
            }
        },
        color
    };
}

} // namespace code
} // namespace qui
