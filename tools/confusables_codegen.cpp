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

// Helper to trim whitespace from both ends
static inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Helper to parse a hex string to char32_t
static char32_t parse_hex(const std::string& s) {
    return static_cast<char32_t>(std::stoul(s, nullptr, 16));
}

// Helper function to escape C++ string literals for header output
std::string escape_cpp_string(const std::string& str) {
    std::ostringstream oss;
    for (unsigned char c : str) {
        if (c == '\\') oss << "\\\\";
        else if (c == '"') oss << "\\\"";
        else if (c == '\n') oss << "\\n";
        else if (c == '\r') oss << "\\r";
        else if (c == '\t') oss << "\\t";
        else if (c < 32 || c > 126) {
            oss << "\\" << std::oct << std::setfill('0') << std::setw(3)
                << static_cast<unsigned int>(c) << std::dec;
        } else {
            oss << static_cast<char>(c);
        }
    }
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_header>\n";
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_header = argv[2];

    std::ifstream ifs(input_file);
    if (!ifs) {
        std::cerr << "Failed to open input file for reading\n";
        return 1;
    }
    std::ofstream ofs_header(output_header);
    if (!ofs_header) {
        std::cerr << "Failed to open output header for writing\n";
        return 1;
    }
    ofs_header << "#pragma once\n\n";
    ofs_header << "// Auto-generated from " << input_file << "\n";
    ofs_header << "#include <unordered_map>\n#include <string>\n\n";

    // First, parse all entries into raw_entries
    std::vector<std::pair<std::string, std::string>> raw_entries;
    std::string line;
    while (std::getline(ifs, line)) {
        // Remove comments
        auto hash_pos = line.find('#');
        if (hash_pos != std::string::npos) line = line.substr(0, hash_pos);
        line = trim(line);
        if (line.empty()) continue;

        // Split on semicolons (with arbitrary whitespace)
        std::vector<std::string> cells;
        size_t pos = 0, next;
        while ((next = line.find(';', pos)) != std::string::npos) {
            cells.push_back(trim(line.substr(pos, next - pos)));
            pos = next + 1;
        }
        cells.push_back(trim(line.substr(pos)));
        if (cells.size() < 2) continue; // skip malformed

        // Parse source codepoint
        std::string src_hex = cells[0];
        if (src_hex.empty()) continue;
        char32_t src = parse_hex(src_hex);
        std::string src_str = unicode_confusables::utf8_utils::codepoint_to_utf8(src);

        // Parse destination sequence as UTF-8 string
        std::istringstream iss(cells[1]);
        std::string codept;
        std::string dst_str;
        while (iss >> codept) {
            char32_t cp = parse_hex(codept);
            dst_str += unicode_confusables::utf8_utils::codepoint_to_utf8(cp);
        }
        if (dst_str.empty()) continue;

        raw_entries.emplace_back(src_str, dst_str);
    }

    // Build equivalence classes using union-find
    std::unordered_map<std::string, std::string> parent;
    std::function<std::string(const std::string&)> find_representative_and_update;
    find_representative_and_update = [&](const std::string& s) -> std::string {
        if (parent.find(s) == parent.end() || parent[s] == s) return s;
        return parent[s] = find_representative_and_update(parent[s]);
    };
    auto unite = [&](const std::string& a, const std::string& b) {
        std::string pa = find_representative_and_update(a);
        std::string pb = find_representative_and_update(b);
        if (pa != pb) {
            if (pa < pb) { // this is the part that ensures that normalized representatives are lexicographically smallest
                parent[pb] = pa;
            } else {
                parent[pa] = pb;
            }
        }
    };
    // Add all pairs (src, dst) as equivalent
    for (const auto& entry : raw_entries) {
        const std::string& src = entry.first;
        const std::string& dst = entry.second;
        unite(src, dst);
    }
    // Collect all members of each class
    std::unordered_map<std::string, std::vector<std::string>> classes;
    for (const auto& entry : parent) {
        std::string rep = find_representative_and_update(entry.first);
        classes[rep].push_back(entry.first);
    }
    // Emit the canonical map directly from classes
    ofs_header << "static const std::unordered_map<std::string, std::string> CONFUSABLES_MAP = {\n";
    size_t count = 0;
    for (const auto& kv : classes) {
        const std::string& canon = kv.first;
        const std::vector<std::string>& members = kv.second;
        for (const std::string& s : members) {
            ofs_header << "    { \"" << escape_cpp_string(s) << "\", \"" << escape_cpp_string(canon) << "\" },\n";
            ++count;
        }
    }
    ofs_header << "};\n";
    ofs_header << "// Entries: " << count << "\n";
    std::cout << "Header generated: " << output_header << " with " << count << " entries.\n";
    return 0;
}
