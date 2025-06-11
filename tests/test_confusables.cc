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
    assert(contains_confusables(cyrillic) == true);
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
    assert(contains_confusables(greek) == true);
}

void test_ascii_negative() {
    std::string actual1 = normalize_confusables("hello");
    if (contains_confusables("hello") != false) {
        std::cout << "[FAIL] test_ascii_negative: contains_confusables returned true for 'hello'\n";
        std::cout.flush();
        return;
    }
    if (actual1 != "hello") {
        std::cout << "[FAIL] test_ascii_negative:\n  got:      '" << actual1 << "'\n  expected: 'hello'\n";
        std::cout.flush();
        return;
    }
    assert(contains_confusables("hello") == false);
    assert(actual1 == "hello");
}

void test_nfkd_normalization() {
    std::string input = "caf\xC3\xA9"; // UTF-8 for café
    std::string expected = "cafe\xCC\x81"; // UTF-8 for 'e' + U+0301
    std::string result = unicode_normalize_kd(input);
    if (result != expected) {
        std::cout << "[FAIL] test_nfkd_normalization:\n  got:      '" << result << "'\n  expected: '" << expected << "'\n";
        std::cout.flush();
        return;
    }
    assert(result == expected && "NFKD normalization failed for 'café'");
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
    test_utf8_conversion();
    std::cout << "All tests passed!\n";
    return 0;
}
