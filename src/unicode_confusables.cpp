#include "unicode_confusables.h"
#include "unicode_confusables_data.h"
#include "utf8_utils.h"
#include <unordered_map>
#include <string>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/errorcode.h>

namespace unicode_confusables {

// Returns true if the string contains any confusable Unicode characters
bool contains_confusables(const std::string& input) {
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    
    for (int32_t i = 0; i < ustr.length(); ) {
        char32_t cp = ustr.char32At(i);
        std::string utf8_char = utf8_utils::codepoint_to_utf8(cp);
        
        if (CONFUSABLES_MAP.find(utf8_char) != CONFUSABLES_MAP.end()) {
            return true;
        }
        
        int32_t charLen = U16_LENGTH(cp);
        i += charLen;
    }
    return false;
}

// Returns a new string with confusable characters replaced by their canonical equivalents
std::string normalize_confusables(const std::string& input) {
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    std::string result;
    
    for (int32_t i = 0; i < ustr.length(); ) {
        char32_t cp = ustr.char32At(i);
        std::string utf8_char = utf8_utils::codepoint_to_utf8(cp);
        
        auto it = CONFUSABLES_MAP.find(utf8_char);
        if (it != CONFUSABLES_MAP.end()) {
            result += it->second;
        } else {
            result += utf8_char;
        }
        
        int32_t charLen = U16_LENGTH(cp);
        i += charLen;
    }
    
    return result;
}

std::string unicode_normalize_kd(const std::string& input) {
    UErrorCode errorCode = U_ZERO_ERROR;
    const icu::Normalizer2* normalizer = icu::Normalizer2::getNFKDInstance(errorCode);
    if (U_FAILURE(errorCode)) {
        return input; // fallback: return input if ICU fails
    }
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    icu::UnicodeString normalized;
    normalizer->normalize(ustr, normalized, errorCode);
    if (U_FAILURE(errorCode)) {
        return input;
    }
    std::string result;
    normalized.toUTF8String(result);
    return result;
}

} // namespace unicode_confusables
