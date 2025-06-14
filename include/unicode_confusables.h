#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/errorcode.h>

namespace unicode_confusables {

// Returns the set of confusable Unicode characters found in the input string
std::unordered_set<std::string> contains_confusables(const std::string& input);

// Returns a new string with confusable characters replaced by their canonical equivalents
std::string normalize_confusables(const std::string& input);

// Returns a new string with NFKD normalization. If strip_zero_width is true, zero-width characters are removed after normalization.
std::string unicode_normalize_kd(const std::string& input, bool strip_zero_width);

} // namespace unicode_confusables
