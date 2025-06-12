#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cctype>
#include "utf8_utils.h"
#include <functional>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>

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
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_header>\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_header = argv[2];

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
    ofs_header << "#pragma once\n\n";
    ofs_header << "// Auto-generated from " << input_file << "\n";
    ofs_header << "#include <unordered_map>\n#include <string>\n\n";

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
        if (dst_str.empty())
            continue;

        // std::cout << "Adding confusable: " << src_str << " -> " << dst_str << "\n";
        raw_entries.emplace_back(src_str, dst_str);
        // std::cout << "Adding confusable: " << dst_str << " -> " << src_str << "\n";
        raw_entries.emplace_back(dst_str, src_str); // add reverse mapping
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

    // Build equivalence classes using union-find
    std::unordered_map<std::string, std::string> parent;
    std::function<std::string(const std::string &)> find_representative_and_update;
    find_representative_and_update = [&](const std::string &s) -> std::string
    {
        if (parent.find(s) == parent.end() || parent[s] == s)
            return s;
        return parent[s] = find_representative_and_update(parent[s]);
    };
    auto unite = [&](const std::string &a, const std::string &b)
    {
        std::string pa = find_representative_and_update(a);
        std::string pb = find_representative_and_update(b);
        if (pa != pb)
        {
            if (pa < pb)
            { 
                // this is the part that ensures that normalized representatives are lexicographically smallest
                parent[pb] = pa;
            }
            else
            {
                parent[pa] = pb;
            }
        }
    };
    // Add all pairs (src, dst) as equivalent
    for (const auto &entry : raw_entries)
    {
        const std::string &src = entry.first;
        const std::string &dst = entry.second;
        unite(src, dst);
    }
    // Collect all members of each class
    std::unordered_map<std::string, std::vector<std::string>> classes;
    for (const auto &entry : parent)
    {
        std::string rep = find_representative_and_update(entry.first);
        classes[rep].push_back(entry.first);
    }
    // Emit the canonical map directly from classes
    ofs_header << "static const std::unordered_map<std::string, std::string> CONFUSABLES_MAP = {\n";
    size_t count = 0;
    for (const auto &kv : classes)
    {
        const std::string &canon = kv.first;
        const std::vector<std::string> &members = kv.second;
        for (const std::string &s : members)
        {
            ofs_header << "    { \"" << escape_cpp_string(s) << "\", \"" << escape_cpp_string(canon) << "\" },\n";
            ++count;
        }
    }
    ofs_header << "};\n";
    ofs_header << "// Entries: " << count << "\n";
    std::cout << "Header generated: " << output_header << " with " << count << " entries.\n";
    return 0;
}
