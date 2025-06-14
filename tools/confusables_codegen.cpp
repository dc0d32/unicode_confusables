#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "utf8_utils.h"
#include <functional>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>

// This tool generates C++ header and source files from a confusables data file.

// The input file should contain lines of the form:
// <source_codepoint_hex>; <destination_codepoint_hex> [<destination_codepoint_hex> ...]
// where source_codepoint_hex is a single Unicode code point in hexadecimal format (e.g., "0041" for 'A'),
// and destination_codepoint_hex is one or more Unicode code points that are visually confusable with the source.
// The output will be two files:
// 1. unicode_confusables_data.h - a header file with declarations of the confusable mappings.
// 2. unicode_confusables_data.cpp - a source file with the actual mappings initialized.
// The generated code will use ICU for Unicode handling and normalization.

static std::unordered_set<char32_t> acceptable_emoji_set = {
    // trademark
    0x2122, // ™
    // registered trademark
    0xAE, // ®
    // copyright
    0xA9, // ©   
};

static inline bool is_ascii(char32_t cp)
{
    return cp < 0x80; // ASCII range is 0x00 to 0x7F
}

// Helper to trim whitespace from both ends
static inline std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Helper to parse a hex string to char32_t
static char32_t parse_hex(const std::string &s)
{
    return static_cast<char32_t>(std::stoul(s, nullptr, 16));
}

// Helper function to escape C++ string literals for header output (no octal, just escape backslash and quote)
std::string escape_cpp_string(const std::string &str)
{
    std::ostringstream oss;
    for (unsigned char c : str)
    {
        if (c == '\\')
            oss << "\\\\";
        else if (c == '"')
            oss << "\\\"";
        else if (c == '\n')
            oss << "\\n";
        else if (c == '\r')
            oss << "\\r";
        else if (c == '\t')
            oss << "\\t";
        else
        {
            oss << static_cast<char>(c);
        }
    }
    return oss.str();
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_header> <output_cpp>\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_header = argv[2];
    std::string output_cpp = argv[3];

    std::ifstream ifs(input_file);
    if (!ifs)
    {
        std::cerr << "Failed to open input file for reading\n";
        return 1;
    }
    std::ofstream ofs_header(output_header);
    if (!ofs_header)
    {
        std::cerr << "Failed to open output header for writing\n";
        return 1;
    }
    std::ofstream ofs_cpp(output_cpp);
    if (!ofs_cpp)
    {
        std::cerr << "Failed to open output cpp file for writing\n";
        return 1;
    }
    
    // Write header file
    ofs_header << "#pragma once\n\n";
    ofs_header << "// Auto-generated from " << input_file << "\n";
    ofs_header << "#include <unordered_map>\n#include <unordered_set>\n#include <string>\n\n";
    ofs_header << "namespace unicode_confusables {\n\n";
    ofs_header << "extern const std::unordered_map<std::string, std::string> CONFUSABLE_TO_CANONICAL;\n";
    ofs_header << "extern const std::unordered_map<std::string, std::unordered_set<std::string>> CONFUSABLES_MAP;\n\n";
    ofs_header << "} // namespace unicode_confusables\n";

    // Write cpp file header
    ofs_cpp << "// Auto-generated from " << input_file << "\n";
    ofs_cpp << "#include \"unicode_confusables_data.h\"\n\n";
    ofs_cpp << "namespace unicode_confusables {\n\n";

    // First, parse all entries into raw_entries
    std::vector<std::pair<std::string, std::string>> raw_entries;
    std::string line;
    while (std::getline(ifs, line))
    {
        // Remove comments
        auto hash_pos = line.find('#');
        if (hash_pos != std::string::npos)
            line = line.substr(0, hash_pos);
        line = trim(line);
        if (line.empty())
            continue;

        // Split on semicolons (with arbitrary whitespace)
        std::vector<std::string> cells;
        size_t pos = 0, next;
        while ((next = line.find(';', pos)) != std::string::npos)
        {
            cells.push_back(trim(line.substr(pos, next - pos)));
            pos = next + 1;
        }
        cells.push_back(trim(line.substr(pos)));
        if (cells.size() < 2)
            continue; // skip malformed

        // Parse source codepoint
        std::string src_hex = cells[0];
        if (src_hex.empty())
            continue;
        char32_t src = parse_hex(src_hex);
        std::string src_str = unicode_confusables::utf8_utils::codepoint_to_utf8(src);

        // Parse destination sequence as UTF-8 string
        std::istringstream iss(cells[1]);
        std::string codept;
        std::string dst_str;
        while (iss >> codept)
        {
            char32_t cp = parse_hex(codept);
            dst_str += unicode_confusables::utf8_utils::codepoint_to_utf8(cp);
        }

        // std::cout << "Adding confusable: " << src_str << " -> " << dst_str << "\n";
        raw_entries.emplace_back(src_str, dst_str);

        // if src_str is single character, add case changed version
        icu::UnicodeString src_unicode_str = icu::UnicodeString::fromUTF8(src_str);
        if (src_unicode_str.length() == 1)
        {
            // using ICU, get the case changed version of src_str. If it is different, add it as well
            auto src_case_changed_str = src_unicode_str.toLower();
            if (src_case_changed_str == src_unicode_str)
            {
                src_case_changed_str = src_unicode_str.toUpper();
            }
            if (src_case_changed_str != src_unicode_str)
            {
                std::string src_case_changed_utf8;
                src_case_changed_str.toUTF8String(src_case_changed_utf8);
                // std::cout << "Adding confusable: " << src_case_changed_utf8 << " -> " << src_str << "\n";
                raw_entries.emplace_back(src_case_changed_utf8, src_str);
            }
        }
        // same for dst_str
        icu::UnicodeString dst_unicode_str = icu::UnicodeString::fromUTF8(dst_str);
        if (dst_unicode_str.length() == 1)
        {
            // using ICU, get the case changed version of dst_str. If it is different, add it as well
            auto dst_case_changed_str = dst_unicode_str.toLower();
            if (dst_case_changed_str == dst_unicode_str)
            {
                dst_case_changed_str = dst_unicode_str.toUpper();
            }
            if (dst_case_changed_str != dst_unicode_str)
            {
                std::string dst_case_changed_utf8;
                dst_case_changed_str.toUTF8String(dst_case_changed_utf8);
                // std::cout << "Adding confusable: " << dst_case_changed_utf8 << " -> " << dst_str << "\n";
                raw_entries.emplace_back(dst_case_changed_utf8, dst_str);
            }
        }
    }

    // also add mappings for accented characters for all ASCII characters
    // Use ICU Normalizer2 for NFD normalization
    UErrorCode status = U_ZERO_ERROR;
    const icu::Normalizer2* normalizer = icu::Normalizer2::getNFDInstance(status);
    if (U_FAILURE(status)) {
        std::cerr << "ICU Normalizer2 NFD instance failed to initialize\n";
        return 1;
    }
    for (char base = 'A'; base <= 'Z'; ++base) {
        std::string base_str(1, base);
        for (UChar32 cp = 0x00A0; cp <= 0x2FFF; ++cp) {
            if ((cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z')) continue;
            icu::UnicodeString ustr(cp);
            icu::UnicodeString nfd = normalizer->normalize(ustr, status);
            if (U_FAILURE(status)) continue;
            std::string nfd_utf8;
            nfd.toUTF8String(nfd_utf8);
            if (nfd_utf8[0] == base) {
                std::string utf8;
                ustr.toUTF8String(utf8);
                // std::cout << "Adding accent confusable: " << utf8 << " -> " << base_str << "\n";
                raw_entries.emplace_back(utf8, base_str);
            }
        }
    }
    for (char base = 'a'; base <= 'z'; ++base) {
        std::string base_str(1, base);
        for (UChar32 cp = 0x00A0; cp <= 0x2FFF; ++cp) {
            if ((cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z')) continue;
            icu::UnicodeString ustr(cp);
            icu::UnicodeString nfd = normalizer->normalize(ustr, status);
            if (U_FAILURE(status)) continue;
            std::string nfd_utf8;
            nfd.toUTF8String(nfd_utf8);
            if (nfd_utf8[0] == base) {
                std::string utf8;
                ustr.toUTF8String(utf8);
                // std::cout << "Adding accent confusable: " << utf8 << " -> " << base_str << "\n";
                raw_entries.emplace_back(utf8, base_str);
            }
        }
    }
    
    // Do not delete normalizer; it is managed by ICU and must not be deleted

    // Map all emojis to a single normalization target (private use character U+E005)
    UErrorCode emojiSetStatus = U_ZERO_ERROR;
    icu::UnicodeSet emojiSet(UNICODE_STRING_SIMPLE("[:Emoji:]"), emojiSetStatus);
    emojiSet.freeze();
    const UChar32 emoji_norm_target = 0xE005; // Private Use Area start
    icu::UnicodeString emoji_norm_target_str(emoji_norm_target);
    std::string emoji_norm_target_utf8;
    emoji_norm_target_str.toUTF8String(emoji_norm_target_utf8);
    for (UChar32 cp = 0; cp <= 0x10FFFF; ++cp) {
        if (!emojiSet.contains(cp) || cp == emoji_norm_target) continue;
        icu::UnicodeString emoji(cp);
        std::string emoji_utf8;
        emoji.toUTF8String(emoji_utf8);
        if (!emoji_utf8.empty()) {
            raw_entries.emplace_back(emoji_utf8, emoji_norm_target_utf8);
        }
        // Also map emoji+VS16 to the normalization target
        icu::UnicodeString emoji_vs(cp);
        emoji_vs.append((UChar32)0xFE0F);
        std::string emoji_vs_utf8;
        emoji_vs.toUTF8String(emoji_vs_utf8);
        if (!emoji_vs_utf8.empty()) {
            raw_entries.emplace_back(emoji_vs_utf8, emoji_norm_target_utf8);
        }
    }

    // Build two mappings:
    // 1. confusable -> canonical (for normalization)
    // 2. canonical -> set of confusables (for lookup operations)
    std::unordered_map<std::string, std::string> confusable_to_canonical;
    std::unordered_map<std::string, std::unordered_set<std::string>> canonical_to_confusables;
    for (const auto& entry : raw_entries) {
        // if src is ASCII, skip it
        // this is required to avoid adding confusables for ASCII characters. The unicode mapping contains some for roman numerals, but we don't want to add those
        // also skip some typical emojis that are not confusable
        if (is_ascii(entry.first[0]) || acceptable_emoji_set.find(unicode_confusables::utf8_utils::get_first_codepoint_from_utf8(entry.first)) != acceptable_emoji_set.end())
            continue;

        confusable_to_canonical[entry.first] = entry.second;
        canonical_to_confusables[entry.second].insert(entry.first);
    }
    // Use runtime initialization instead of large initializer lists for better compile times
    size_t count1 = confusable_to_canonical.size();
    size_t count2 = 0;
    for (const auto& kv : canonical_to_confusables) {
        count2 += kv.second.size();
    }
    
    // Split data initialization into chunks to improve compilation time
    const size_t CHUNK_SIZE = 500;  // Reduced chunk size for even better compilation performance
    size_t chunk_num = 0;
    
    // Generate initialization functions for CONFUSABLE_TO_CANONICAL
    std::vector<std::vector<std::pair<std::string, std::string>>> confusable_chunks;
    auto it = confusable_to_canonical.begin();
    while (it != confusable_to_canonical.end()) {
        confusable_chunks.emplace_back();
        for (size_t i = 0; i < CHUNK_SIZE && it != confusable_to_canonical.end(); ++i, ++it) {
            confusable_chunks.back().emplace_back(it->first, it->second);
        }
    }
    
    // Generate chunk initialization functions
    for (size_t i = 0; i < confusable_chunks.size(); ++i) {
        ofs_cpp << "static void init_confusable_to_canonical_chunk_" << i << "(std::unordered_map<std::string, std::string>& map) {\n";
        for (const auto& kv : confusable_chunks[i]) {
            ofs_cpp << "    map[\"" << escape_cpp_string(kv.first) << "\"] = \"" << escape_cpp_string(kv.second) << "\";\n";
        }
        ofs_cpp << "}\n\n";
    }
    
    // Generate the main map with runtime initialization
    ofs_cpp << "const std::unordered_map<std::string, std::string> CONFUSABLE_TO_CANONICAL = []() {\n";
    ofs_cpp << "    std::unordered_map<std::string, std::string> map;\n";
    ofs_cpp << "    map.reserve(" << count1 << ");\n";
    for (size_t i = 0; i < confusable_chunks.size(); ++i) {
        ofs_cpp << "    init_confusable_to_canonical_chunk_" << i << "(map);\n";
    }
    ofs_cpp << "    return map;\n";
    ofs_cpp << "}();\n\n";
    
    // Generate initialization functions for CONFUSABLES_MAP
    std::vector<std::vector<std::pair<std::string, std::unordered_set<std::string>>>> confusables_map_chunks;
    auto it2 = canonical_to_confusables.begin();
    while (it2 != canonical_to_confusables.end()) {
        confusables_map_chunks.emplace_back();
        for (size_t i = 0; i < CHUNK_SIZE && it2 != canonical_to_confusables.end(); ++i, ++it2) {
            confusables_map_chunks.back().emplace_back(it2->first, it2->second);
        }
    }
    
    // Generate chunk initialization functions for CONFUSABLES_MAP
    for (size_t i = 0; i < confusables_map_chunks.size(); ++i) {
        ofs_cpp << "static void init_confusables_map_chunk_" << i << "(std::unordered_map<std::string, std::unordered_set<std::string>>& map) {\n";
        for (const auto& kv : confusables_map_chunks[i]) {
            ofs_cpp << "    map[\"" << escape_cpp_string(kv.first) << "\"] = { ";
            bool first = true;
            for (const std::string& s : kv.second) {
                if (!first) ofs_cpp << ", ";
                ofs_cpp << "\"" << escape_cpp_string(s) << "\"";
                first = false;
            }
            ofs_cpp << " };\n";
        }
        ofs_cpp << "}\n\n";
    }
    
    // Generate the main CONFUSABLES_MAP with runtime initialization
    ofs_cpp << "const std::unordered_map<std::string, std::unordered_set<std::string>> CONFUSABLES_MAP = []() {\n";
    ofs_cpp << "    std::unordered_map<std::string, std::unordered_set<std::string>> map;\n";
    ofs_cpp << "    map.reserve(" << canonical_to_confusables.size() << ");\n";
    for (size_t i = 0; i < confusables_map_chunks.size(); ++i) {
        ofs_cpp << "    init_confusables_map_chunk_" << i << "(map);\n";
    }
    ofs_cpp << "    return map;\n";
    ofs_cpp << "}();\n\n";
    
    ofs_cpp << "} // namespace unicode_confusables\n";
    ofs_cpp << "// Confusable->Canonical entries: " << count1 << ", Canonical->Confusables entries: " << count2 << "\n";
    std::cout << "Files generated: " << output_header << " and " << output_cpp << " with " << count1 << " confusable mappings and " << count2 << " confusable entries.\n";
    return 0;
}
