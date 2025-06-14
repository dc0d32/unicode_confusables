#include "unicode_confusables_c.h"
#include "../../include/unicode_confusables.h"
#include <vector>
#include <cstring>
#include <cstdlib>

// Helper struct for storing confusables set
struct ConfusablesSet {
    std::vector<std::string> items;
};

extern "C" {

ConfusablesSetHandle unicode_confusables_contains_confusables(const char* input) {
    if (!input) return nullptr;
    
    try {
        auto confusables = unicode_confusables::contains_confusables(std::string(input));
        auto* set = new ConfusablesSet();
        set->items.reserve(confusables.size());
        
        for (const auto& item : confusables) {
            set->items.push_back(item);
        }
        
        return set;
    } catch (...) {
        return nullptr;
    }
}

void unicode_confusables_free_set(ConfusablesSetHandle handle) {
    delete handle;
}

int unicode_confusables_set_size(ConfusablesSetHandle handle) {
    if (!handle) return 0;
    return static_cast<int>(handle->items.size());
}

const char* unicode_confusables_set_get(ConfusablesSetHandle handle, int index) {
    if (!handle || index < 0 || index >= static_cast<int>(handle->items.size())) {
        return nullptr;
    }
    return handle->items[index].c_str();
}

char* unicode_confusables_normalize_confusables(const char* input) {
    if (!input) return nullptr;
    
    try {
        std::string result = unicode_confusables::normalize_confusables(std::string(input));
        char* c_result = static_cast<char*>(malloc(result.length() + 1));
        if (c_result) {
            strcpy(c_result, result.c_str());
        }
        return c_result;
    } catch (...) {
        return nullptr;
    }
}

char* unicode_confusables_unicode_normalize(const char* input, int type, int strip_zero_width) {
    if (!input) return nullptr;
    
    try {
        unicode_confusables::NormalizationType norm_type;
        switch (type) {
            case 0: norm_type = unicode_confusables::NormalizationType::NFC; break;
            case 1: norm_type = unicode_confusables::NormalizationType::NFD; break;
            case 2: norm_type = unicode_confusables::NormalizationType::NFKC; break;
            case 3: norm_type = unicode_confusables::NormalizationType::NFKD; break;
            default: return nullptr;
        }
        
        std::string result = unicode_confusables::unicode_normalize(std::string(input), norm_type, strip_zero_width != 0);
        char* c_result = static_cast<char*>(malloc(result.length() + 1));
        if (c_result) {
            strcpy(c_result, result.c_str());
        }
        return c_result;
    } catch (...) {
        return nullptr;
    }
}

void unicode_confusables_free_string(char* str) {
    free(str);
}

}
