#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cctype>

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
    ofs_header << "#include <unordered_map>\n#include <vector>\n#include <cstdint>\n\n";
    ofs_header << "static const std::unordered_map<char32_t, std::vector<char32_t>> CONFUSABLES_MAP = {\n";

    std::string line;
    size_t count = 0;
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

        // Parse destination sequence as vector<char32_t>
        std::istringstream iss(cells[1]);
        std::string codept;
        std::vector<char32_t> dst;
        while (iss >> codept) {
            dst.push_back(parse_hex(codept));
        }
        if (dst.empty()) continue;

        // Write entry
        ofs_header << "    { 0x" << std::hex << std::uppercase << src << ", { ";
        for (size_t i = 0; i < dst.size(); ++i) {
            if (i > 0) ofs_header << ", ";
            ofs_header << "0x" << std::hex << std::uppercase << dst[i];
        }
        ofs_header << " } },\n";
        ++count;
    }
    ofs_header << "};\n";
    ofs_header << "// Entries: " << count << "\n";
    std::cout << "Header generated: " << output_header << " with " << count << " entries.\n";
    return 0;
}
