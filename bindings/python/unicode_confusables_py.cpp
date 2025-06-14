#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "../../include/unicode_confusables.h"

namespace py = pybind11;

PYBIND11_MODULE(unicode_confusables_py, m) {
    m.doc() = "Python bindings for Unicode Confusables detection and normalization";
    
    // Bind the NormalizationType enum
    py::enum_<unicode_confusables::NormalizationType>(m, "NormalizationType")
        .value("NFC", unicode_confusables::NormalizationType::NFC, "Normalization Form Composed")
        .value("NFD", unicode_confusables::NormalizationType::NFD, "Normalization Form Decomposed")
        .value("NFKC", unicode_confusables::NormalizationType::NFKC, "Normalization Form Compatibility Composed")
        .value("NFKD", unicode_confusables::NormalizationType::NFKD, "Normalization Form Compatibility Decomposed");
    
    m.def("contains_confusables", &unicode_confusables::contains_confusables,
          "Returns the set of confusable Unicode characters found in the input string",
          py::arg("input"));
    
    m.def("normalize_confusables", &unicode_confusables::normalize_confusables,
          "Returns a new string with confusable characters replaced by their canonical equivalents",
          py::arg("input"));
    
    m.def("unicode_normalize", &unicode_confusables::unicode_normalize,
          "Returns a new string with Unicode normalization applied. If strip_zero_width is True, zero-width characters are removed after normalization.",
          py::arg("input"), py::arg("type"), py::arg("strip_zero_width") = false);
    
    // Keep the old function for backward compatibility
    m.def("unicode_normalize_kd", [](const std::string& input, bool strip_zero_width) {
        return unicode_confusables::unicode_normalize(input, unicode_confusables::NormalizationType::NFKD, strip_zero_width);
    }, "Returns a new string with NFKD normalization. If strip_zero_width is True, zero-width characters are removed after normalization. (Deprecated: use unicode_normalize with NormalizationType.NFKD)",
       py::arg("input"), py::arg("strip_zero_width") = false);
}
