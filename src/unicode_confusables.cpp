#include "unicode_confusables.h"
#include "unicode_confusables_data.h"
#include "utf8_utils.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/errorcode.h>

namespace unicode_confusables {

// Helper function to strip zero-width characters from a normalized string
static std::string strip_zero_width_chars(const icu::UnicodeString& normalized) {
    UErrorCode setStatus = U_ZERO_ERROR;
    icu::UnicodeSet zwSet(UNICODE_STRING_SIMPLE("[:Cf:]"), setStatus);
    zwSet.freeze();
    icu::UnicodeString filtered;
    for (int32_t i = 0; i < normalized.length(); ) {
        UChar32 cp = normalized.char32At(i);
        if (!zwSet.contains(cp)) {
            filtered.append(cp);
        }
        i += U16_LENGTH(cp);
    }
    std::string result;
    filtered.toUTF8String(result);
    return result;
}

// Returns the set of confusable Unicode characters found in the input string
std::unordered_set<std::string> contains_confusables(const std::string& input) {
    std::unordered_set<std::string> confusables_found;
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    
    for (int32_t i = 0; i < ustr.length(); ) {
        char32_t cp = ustr.char32At(i);
        std::string utf8_char = utf8_utils::codepoint_to_utf8(cp);
        
        if (CONFUSABLE_TO_CANONICAL.find(utf8_char) != CONFUSABLE_TO_CANONICAL.end()) {
            confusables_found.insert(utf8_char);
        }
        
        int32_t charLen = U16_LENGTH(cp);
        i += charLen;
    }
    return confusables_found;
}

// Returns a new string with confusable characters replaced by their canonical equivalents
std::string normalize_confusables(const std::string& input) {
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    std::string result;
    
    for (int32_t i = 0; i < ustr.length(); ) {
        char32_t cp = ustr.char32At(i);
        std::string utf8_char = utf8_utils::codepoint_to_utf8(cp);
        
        auto it = CONFUSABLE_TO_CANONICAL.find(utf8_char);
        if (it != CONFUSABLE_TO_CANONICAL.end()) {
            result += it->second;
        } else {
            result += utf8_char;
        }
        
        int32_t charLen = U16_LENGTH(cp);
        i += charLen;
    }
    
    return result;
}

std::string unicode_normalize(const std::string& input, NormalizationType type, bool strip_zero_width) {
    UErrorCode errorCode = U_ZERO_ERROR;
    const icu::Normalizer2* normalizer = nullptr;
    
    // Get the appropriate normalizer based on the type
    switch (type) {
        case NormalizationType::NFC:
            normalizer = icu::Normalizer2::getNFCInstance(errorCode);
            break;
        case NormalizationType::NFD:
            normalizer = icu::Normalizer2::getNFDInstance(errorCode);
            break;
        case NormalizationType::NFKC:
            normalizer = icu::Normalizer2::getNFKCInstance(errorCode);
            break;
        case NormalizationType::NFKD:
            normalizer = icu::Normalizer2::getNFKDInstance(errorCode);
            break;
    }
    
    if (U_FAILURE(errorCode) || normalizer == nullptr) {
        return input; // fallback: return input if ICU fails
    }
    
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    icu::UnicodeString normalized;
    normalizer->normalize(ustr, normalized, errorCode);
    if (U_FAILURE(errorCode)) {
        return input;
    }
    
    if (strip_zero_width) {
        return strip_zero_width_chars(normalized);
    } else {
        std::string result;
        normalized.toUTF8String(result);
        return result;
    }
}

} // namespace unicode_confusables
