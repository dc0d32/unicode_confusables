#pragma once
#include <string>
#include <unordered_map>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/errorcode.h>

namespace unicode_confusables {

// Returns true if the string contains any confusable Unicode characters
bool contains_confusables(const std::string& input);

// Returns a new string with confusable characters replaced by their canonical equivalents
std::string normalize_confusables(const std::string& input);

std::string unicode_normalize_kd(const std::string& input);

} // namespace unicode_confusables
