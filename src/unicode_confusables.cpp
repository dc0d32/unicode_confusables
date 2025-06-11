#include "unicode_confusables.h"
#include "unicode_confusables_data.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/errorcode.h>

namespace unicode_confusables {

// Returns true if the string contains any confusable Unicode characters
bool contains_confusables(const std::string& input) {
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    for (int32_t i = 0; i < ustr.length(); ++i) {
        char32_t cp = ustr.char32At(i);
        if (CONFUSABLES_MAP.find(cp) != CONFUSABLES_MAP.end()) {
            return true;
        }
        // If surrogate pair, skip the next code unit
        if (U_IS_LEAD(ustr[i]) && i + 1 < ustr.length() && U_IS_TRAIL(ustr[i+1])) {
            ++i;
        }
    }
    return false;
}

// Returns a new string with confusable characters replaced by their canonical equivalents
std::string normalize_confusables(const std::string& input) {
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(input);
    icu::UnicodeString out;
    for (int32_t i = 0; i < ustr.length(); ) {
        char32_t cp = ustr.char32At(i);
        auto it = CONFUSABLES_MAP.find(cp);
        if (it != CONFUSABLES_MAP.end()) {
            for (char32_t mapped : it->second) {
                out.append(static_cast<UChar32>(mapped));
            }
        } else {
            out.append(static_cast<UChar32>(cp));
        }
        int32_t charLen = U16_LENGTH(cp);
        i += charLen;
    }
    std::string result;
    out.toUTF8String(result);
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
