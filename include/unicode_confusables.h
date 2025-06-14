#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/errorcode.h>

namespace unicode_confusables {

// Unicode normalization types
enum class NormalizationType {
    NFC,   // Normalization Form Composed
    NFD,   // Normalization Form Decomposed  
    NFKC,  // Normalization Form Compatibility Composed
    NFKD   // Normalization Form Compatibility Decomposed
};

// Returns the set of confusable Unicode characters found in the input string
std::unordered_set<std::string> contains_confusables(const std::string& input);

// Returns a new string with confusable characters replaced by their canonical equivalents
std::string normalize_confusables(const std::string& input);

// Returns a new string with Unicode normalization applied. If strip_zero_width is true, zero-width characters are removed after normalization.
std::string unicode_normalize(const std::string& input, NormalizationType type, bool strip_zero_width);

} // namespace unicode_confusables
