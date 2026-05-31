#pragma once

#include <rapidfuzz/fuzz.hpp>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <execution>
#include <string>
#include <string_view>
#include <vector>

namespace qui {

struct fuzzy_match {
    std::size_t index = 0;
    double score = 0.0;
};

inline std::string fuzzy_normalize(std::string_view text) {
    std::string normalized;
    normalized.reserve(text.size());
    for (const unsigned char ch : text) {
        normalized.push_back(static_cast<char>(std::tolower(ch)));
    }
    return normalized;
}

inline std::vector<fuzzy_match> fuzzy_search(
    std::string_view query,
    const std::vector<std::string> &choices,
    double cutoff = 60.0
) {
    std::vector<fuzzy_match> matches;
    if (query.empty()) {
        matches.reserve(choices.size());
        for (std::size_t i = 0; i < choices.size(); ++i) {
            matches.push_back({i, 100.0});
        }
        return matches;
    }

    const std::string normalized_query = fuzzy_normalize(query);

    matches.resize(choices.size());
    std::transform(
        std::execution::par,
        choices.begin(),
        choices.end(),
        matches.begin(),
        [cutoff, &normalized_query, base = choices.data()](const std::string &choice) {
            const std::string normalized_choice = fuzzy_normalize(choice);
            const double score = rapidfuzz::fuzz::WRatio(normalized_query, normalized_choice, cutoff);
            return fuzzy_match{static_cast<std::size_t>(&choice - base), score};
        }
    );

    matches.erase(std::remove_if(matches.begin(), matches.end(), [cutoff](const fuzzy_match &match) {
        return match.score < cutoff;
    }), matches.end());

    std::stable_sort(matches.begin(), matches.end(), [](const fuzzy_match &lhs, const fuzzy_match &rhs) {
        return lhs.score > rhs.score;
    });
    return matches;
}

} // namespace qui
