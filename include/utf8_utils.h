#pragma once

#include <string>
#include <cstdint>

namespace unicode_confusables {
namespace utf8_utils {

/**
 * Convert a Unicode codepoint to a UTF-8 encoded string.
 * @param cp The Unicode codepoint to convert
 * @return UTF-8 encoded string representation of the codepoint
 */
inline std::string codepoint_to_utf8(char32_t cp) {
    std::string result;
    if (cp <= 0x7F) {
        result += static_cast<char>(cp);
    } else if (cp <= 0x7FF) {
        result += static_cast<char>(0xC0 | (cp >> 6));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) {
        result += static_cast<char>(0xE0 | (cp >> 12));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0x10FFFF) {
        result += static_cast<char>(0xF0 | (cp >> 18));
        result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    return result;
}

// Decodes the next codepoint from a UTF-8 string, advancing the index. Returns U+FFFD on error.
inline char32_t next_codepoint(const std::string& str, size_t& i) {
    if (i >= str.size()) return 0xFFFD;
    unsigned char c = str[i];
    if (c < 0x80) {
        return str[i++];
    } else if ((c >> 5) == 0x6 && i + 1 < str.size()) {
        char32_t cp = ((str[i] & 0x1F) << 6) | (str[i+1] & 0x3F);
        i += 2;
        return cp;
    } else if ((c >> 4) == 0xE && i + 2 < str.size()) {
        char32_t cp = ((str[i] & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
        i += 3;
        return cp;
    } else if ((c >> 3) == 0x1E && i + 3 < str.size()) {
        char32_t cp = ((str[i] & 0x07) << 18) | ((str[i+1] & 0x3F) << 12) |
                      ((str[i+2] & 0x3F) << 6) | (str[i+3] & 0x3F);
        i += 4;
        return cp;
    }
    // Invalid, skip
    ++i;
    return 0xFFFD;
}

/**
 * Extract the first Unicode codepoint from a UTF-8 encoded string.
 * @param str The UTF-8 encoded string
 * @return The first Unicode codepoint, or U+FFFD on error
 */
inline char32_t get_first_codepoint_from_utf8(const std::string& str) {
    size_t i = 0;
    return next_codepoint(str, i);
}

} // namespace utf8_utils
} // namespace unicode_confusables
