#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handle for confusables result set
typedef struct ConfusablesSet* ConfusablesSetHandle;

// C API functions
ConfusablesSetHandle unicode_confusables_contains_confusables(const char* input);
void unicode_confusables_free_set(ConfusablesSetHandle handle);
int unicode_confusables_set_size(ConfusablesSetHandle handle);
const char* unicode_confusables_set_get(ConfusablesSetHandle handle, int index);

char* unicode_confusables_normalize_confusables(const char* input);
char* unicode_confusables_unicode_normalize_kd(const char* input, int strip_zero_width);
void unicode_confusables_free_string(char* str);

#ifdef __cplusplus
}
#endif
