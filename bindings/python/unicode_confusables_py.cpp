#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "../../include/unicode_confusables.h"

namespace py = pybind11;

PYBIND11_MODULE(unicode_confusables_py, m) {
    m.doc() = "Python bindings for Unicode Confusables detection and normalization";
    
    m.def("contains_confusables", &unicode_confusables::contains_confusables,
          "Returns the set of confusable Unicode characters found in the input string",
          py::arg("input"));
    
    m.def("normalize_confusables", &unicode_confusables::normalize_confusables,
          "Returns a new string with confusable characters replaced by their canonical equivalents",
          py::arg("input"));
    
    m.def("unicode_normalize_kd", &unicode_confusables::unicode_normalize_kd,
          "Returns a new string with NFKD normalization. If strip_zero_width is True, zero-width characters are removed after normalization.",
          py::arg("input"), py::arg("strip_zero_width") = false);
}
