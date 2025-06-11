#include "unicode_confusables.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Check for help flag
    if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        std::cout << "Usage: " << argv[0] << " [options]\n";
        std::cout << "Normalizes Unicode confusable characters from stdin to stdout.\n\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h     Show this help message\n";
        std::cout << "  --check, -c    Only check if input contains confusables (exit code 0=clean, 1=contains confusables)\n";
        std::cout << "  --nfkd, -n     Apply NFKD normalization before confusables normalization\n";
        std::cout << "\nExamples:\n";
        std::cout << "  echo 'Hello Wοrld' | " << argv[0] << "\n";
        std::cout << "  echo 'café' | " << argv[0] << " --nfkd\n";
        std::cout << "  echo 'suspicious text' | " << argv[0] << " --check\n";
        return 0;
    }

    bool check_only = false;
    bool use_nfkd = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--check" || arg == "-c") {
            check_only = true;
        } else if (arg == "--nfkd" || arg == "-n") {
            use_nfkd = true;
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
            if (unicode_confusables::contains_confusables(line)) {
                std::cout << "CONFUSABLES_DETECTED\n";
                exit_code = 1;
            } else {
                std::cout << "CLEAN\n";
            }
        } else {
            if (use_nfkd) {
                // Apply NFKD normalization
                line = unicode_confusables::unicode_normalize_kd(line);
            }
            // Apply confusables normalization (default)
            line = unicode_confusables::normalize_confusables(line);
            std::cout << line << "\n";
        }
    }

    return exit_code;
}
