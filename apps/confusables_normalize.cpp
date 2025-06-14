#include "unicode_confusables.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Check for help flag
    if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        std::cout << "Usage: " << argv[0] << " [options]\n";
        std::cout << "Normalizes Unicode confusable characters from stdin to stdout.\n\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h              Show this help message\n";
        std::cout << "  --check, -c             Only check if input contains confusables (exit code 0=clean, 1=contains confusables)\n";
        std::cout << "  --normalize, -n TYPE    Apply Unicode normalization before confusables normalization\n";
        std::cout << "                          TYPE can be: nfc, nfd, nfkc, nfkd, none (default: none)\n";
        std::cout << "\nExamples:\n";
        std::cout << "  echo 'Hello Wοrld' | " << argv[0] << "\n";
        std::cout << "  echo 'café' | " << argv[0] << " --normalize nfd\n";
        std::cout << "  echo 'café' | " << argv[0] << " -n nfkd\n";
        std::cout << "  echo 'ﬁle' | " << argv[0] << " --normalize nfkc\n";
        std::cout << "  echo 'suspicious text' | " << argv[0] << " --check\n";
        return 0;
    }

    bool check_only = false;
    std::string normalization_type = "none";

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--check" || arg == "-c") {
            check_only = true;
        } else if (arg == "--normalize" || arg == "-n") {
            // Check if there's a next argument for the normalization type
            if (i + 1 >= argc) {
                std::cerr << "Error: --normalize requires a type (nfc, nfd, nfkc, nfkd, or none)\n";
                std::cerr << "Use --help for usage information.\n";
                return 1;
            }
            normalization_type = argv[++i];
            if (normalization_type != "nfc" && normalization_type != "nfd" && 
                normalization_type != "nfkc" && normalization_type != "nfkd" && 
                normalization_type != "none") {
                std::cerr << "Error: Invalid normalization type '" << normalization_type << "'\n";
                std::cerr << "Valid types are: nfc, nfd, nfkc, nfkd, none\n";
                return 1;
            }
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            std::cerr << "Use --help for usage information.\n";
            return 1;
        }
    }

    std::string line;
    int exit_code = 0;

    // Read input line by line
    while (std::getline(std::cin, line)) {
        if (check_only) {
            // Just check for confusables
            auto confusables = unicode_confusables::contains_confusables(line);
            if (!confusables.empty()) {
                std::cout << "CONFUSABLES_DETECTED: ";
                bool first = true;
                for (const auto& confusable : confusables) {
                    if (!first) std::cout << ", ";
                    std::cout << "'" << confusable << "'";
                    first = false;
                }
                std::cout << "\n";
                exit_code = 1;
            } else {
                std::cout << "CLEAN\n";
            }
        } else {
            if (normalization_type != "none") {
                // Apply Unicode normalization
                unicode_confusables::NormalizationType norm_type;
                if (normalization_type == "nfc") {
                    norm_type = unicode_confusables::NormalizationType::NFC;
                } else if (normalization_type == "nfd") {
                    norm_type = unicode_confusables::NormalizationType::NFD;
                } else if (normalization_type == "nfkc") {
                    norm_type = unicode_confusables::NormalizationType::NFKC;
                } else if (normalization_type == "nfkd") {
                    norm_type = unicode_confusables::NormalizationType::NFKD;
                }
                line = unicode_confusables::unicode_normalize(line, norm_type, true);
            }
            // Apply confusables normalization (default)
            line = unicode_confusables::normalize_confusables(line);
            std::cout << line << "\n";
        }
    }

    return exit_code;
}
