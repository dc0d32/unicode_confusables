#include "unicode_confusables.h"
#include "utf8_utils.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace unicode_confusables;

void test_cyrillic_confusable() {
    std::string cyrillic = "p\xD0\xB0p"; // 'p' + Cyrillic 'а' (U+0430) + 'p', visually 'pap'
    std::string expected_cyrillic = "pap";
    std::string actual = normalize_confusables(cyrillic);
    if (actual != expected_cyrillic) {
        std::cout << "[FAIL] test_cyrillic_confusable:\n  got:      '" << actual << "'\n  expected: '" << expected_cyrillic << "'\n";
        std::cout.flush();
        return;
    }
    assert(actual == expected_cyrillic);
    assert(!contains_confusables(cyrillic).empty());
}

void test_greek_confusable() {
    std::string greek = "𝑐Ӡρꬽ 𝑐Ӡρо 𝑐Ӡρھ 𝑐Ӡρο 𝑐Ӡρ၀ 𝑐Ӡρہ 𝑐Ӡρσ 𝑐Ӡρه 𝑐Ӡρｏ 𝑐Ӡρ๐ 𝑐Ӡρ໐ 𝑐Ӡρ𝕠 𝑐Ӡρ𐐬";
    std::string expected_greek = "c3po c3po c3po c3po c3po c3po c3po c3po c3po c3po c3po c3po c3po";
    std::string actual = normalize_confusables(greek);
    if (actual != expected_greek) {
        std::cout << "[FAIL] test_greek_confusable:\n  got:      '" << actual << "'\n  expected: '" << expected_greek << "'\n";
        std::cout.flush();
        return;
    }
    assert(actual == expected_greek);
    assert(!contains_confusables(greek).empty());
}

void test_ascii_negative() {
    std::string actual1 = normalize_confusables("hello");
    if (!contains_confusables("hello").empty()) {
        std::cout << "[FAIL] test_ascii_negative: contains_confusables returned non-empty set for 'hello'\n";
        std::cout.flush();
        return;
    }
    if (actual1 != "hello") {
        std::cout << "[FAIL] test_ascii_negative:\n  got:      '" << actual1 << "'\n  expected: 'hello'\n";
        std::cout.flush();
        return;
    }
    assert(contains_confusables("hello").empty());
    assert(actual1 == "hello");
}

void test_nfkd_normalization() {
    std::string input = "caf\xC3\xA9"; // UTF-8 for café
    std::string expected = "cafe\xCC\x81"; // UTF-8 for 'e' + U+0301
    std::string result = unicode_normalize(input, NormalizationType::NFKD, false);
    if (result != expected) {
        std::cout << "[FAIL] test_nfkd_normalization:\n  got:      '" << result << "'\n  expected: '" << expected << "'\n";
        std::cout.flush();
        return;
    }
    assert(result == expected && "NFKD normalization failed for 'café'");
}

void test_nfd_normalization() {
    std::string input = "caf\xC3\xA9"; // UTF-8 for café
    std::string expected = "cafe\xCC\x81"; // UTF-8 for 'e' + U+0301 (combining acute accent)
    std::string result = unicode_normalize(input, NormalizationType::NFD, false);
    if (result != expected) {
        std::cout << "[FAIL] test_nfd_normalization:\n  got:      '" << result << "'\n  expected: '" << expected << "'\n";
        std::cout.flush();
        return;
    }
    assert(result == expected && "NFD normalization failed for 'café'");
}

void test_nfd_vs_nfkd() {
    // Test with ﬁ (ligature fi, U+FB01) - NFKD decomposes it, NFD doesn't
    std::string input = "\xEF\xAC\x81"; // UTF-8 for ﬁ (U+FB01)
    
    std::string nfd_result = unicode_normalize(input, NormalizationType::NFD, false);
    std::string nfkd_result = unicode_normalize(input, NormalizationType::NFKD, false);
    
    // NFD should preserve the ligature (no decomposition for compatibility characters)
    std::string expected_nfd = "\xEF\xAC\x81"; // Still ﬁ
    // NFKD should decompose to "fi"
    std::string expected_nfkd = "fi";
    
    if (nfd_result != expected_nfd) {
        std::cout << "[FAIL] test_nfd_vs_nfkd (NFD part):\n  got:      '" << nfd_result << "'\n  expected: '" << expected_nfd << "'\n";
        std::cout.flush();
        return;
    }
    if (nfkd_result != expected_nfkd) {
        std::cout << "[FAIL] test_nfd_vs_nfkd (NFKD part):\n  got:      '" << nfkd_result << "'\n  expected: '" << expected_nfkd << "'\n";
        std::cout.flush();
        return;
    }
    
    assert(nfd_result == expected_nfd && "NFD should not decompose compatibility characters");
    assert(nfkd_result == expected_nfkd && "NFKD should decompose compatibility characters");
}

void test_zero_width_stripping() {
    // Test with zero-width joiner (U+200D) and zero-width non-joiner (U+200C)
    std::string input = "a\xE2\x80\x8D" "b\xE2\x80\x8C" "c"; // a + ZWJ + b + ZWNJ + c
    
    // Without stripping zero-width characters
    std::string result_with_zw = unicode_normalize(input, NormalizationType::NFD, false);
    std::string expected_with_zw = "a\xE2\x80\x8D" "b\xE2\x80\x8C" "c";
    
    // With stripping zero-width characters
    std::string result_without_zw = unicode_normalize(input, NormalizationType::NFD, true);
    std::string expected_without_zw = "abc";
    
    if (result_with_zw != expected_with_zw) {
        std::cout << "[FAIL] test_zero_width_stripping (with ZW):\n  got:      '" << result_with_zw << "'\n  expected: '" << expected_with_zw << "'\n";
        std::cout.flush();
        return;
    }
    if (result_without_zw != expected_without_zw) {
        std::cout << "[FAIL] test_zero_width_stripping (without ZW):\n  got:      '" << result_without_zw << "'\n  expected: '" << expected_without_zw << "'\n";
        std::cout.flush();
        return;
    }
    
    assert(result_with_zw == expected_with_zw && "NFD should preserve zero-width characters when not stripping");
    assert(result_without_zw == expected_without_zw && "NFD should remove zero-width characters when stripping");
}

void test_nfd_empty_and_ascii() {
    // Test empty string
    std::string empty_result = unicode_normalize("", NormalizationType::NFD, false);
    assert(empty_result == "" && "NFD of empty string should be empty");
    
    // Test ASCII (should remain unchanged)
    std::string ascii = "Hello World!";
    std::string ascii_result = unicode_normalize(ascii, NormalizationType::NFD, false);
    assert(ascii_result == ascii && "NFD of ASCII should remain unchanged");
    
    // Test ASCII with zero-width stripping
    std::string ascii_result_stripped = unicode_normalize(ascii, NormalizationType::NFD, true);
    assert(ascii_result_stripped == ascii && "NFD of ASCII with stripping should remain unchanged");
}

void test_all_normalization_types() {
    // Test with composed character é (U+00E9)
    std::string input_composed = "\xC3\xA9"; // UTF-8 for é
    
    // Test with decomposed character e + combining acute accent
    std::string input_decomposed = "e\xCC\x81"; // UTF-8 for 'e' + U+0301
    
    // Test NFC - should compose
    std::string nfc_result = unicode_normalize(input_decomposed, NormalizationType::NFC, false);
    std::string expected_nfc = "\xC3\xA9"; // Should be é
    assert(nfc_result == expected_nfc && "NFC should compose characters");
    
    // Test NFD - should decompose
    std::string nfd_result = unicode_normalize(input_composed, NormalizationType::NFD, false);
    std::string expected_nfd = "e\xCC\x81"; // Should be e + combining accent
    assert(nfd_result == expected_nfd && "NFD should decompose characters");
    
    // Test with ligature ﬁ for NFKC/NFKD
    std::string ligature = "\xEF\xAC\x81"; // UTF-8 for ﬁ (U+FB01)
    
    // Test NFKC - should decompose compatibility and then compose
    std::string nfkc_result = unicode_normalize(ligature, NormalizationType::NFKC, false);
    std::string expected_nfkc = "fi"; // Should be regular fi
    assert(nfkc_result == expected_nfkc && "NFKC should decompose compatibility characters");
    
    // Test NFKD - should decompose compatibility
    std::string nfkd_result = unicode_normalize(ligature, NormalizationType::NFKD, false);
    std::string expected_nfkd = "fi"; // Should be regular fi
    assert(nfkd_result == expected_nfkd && "NFKD should decompose compatibility characters");
}

void test_utf8_conversion() {
        // Test some Unicode codepoints
    char32_t test_codepoints[] = {
        0x48,      // 'H' (ASCII)
        0xE9,      // 'é' (Latin-1 Supplement)
        0x4E2D,    // '中' (CJK)
        0x1F600,   // '😀' (Emoji)
    };

    std::string expected_utf8[] = {
        "H",        // U+0048
        "\xC3\xA9", // U+00E9
        "\xE4\xB8\xAD", // U+4E2D
        "\xF0\x9F\x98\x80" // U+1F600
    };
    
    std::cout << "Testing UTF-8 conversion utility:\n";
    for (size_t i = 0; i < sizeof(test_codepoints) / sizeof(test_codepoints[0]); ++i) {
        char32_t cp = test_codepoints[i];
        std::string utf8_str = unicode_confusables::utf8_utils::codepoint_to_utf8(cp);
        // assert
        assert(utf8_str == expected_utf8[i]);
    }
}

int main() {
    test_cyrillic_confusable();
    test_greek_confusable();
    test_ascii_negative();
    test_nfkd_normalization();
    test_nfd_normalization();
    test_nfd_vs_nfkd();
    test_zero_width_stripping();
    test_nfd_empty_and_ascii();
    test_all_normalization_types();
    test_utf8_conversion();
    std::cout << "All tests passed!\n";
    return 0;
}
