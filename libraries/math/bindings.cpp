// #include <pybind11/pybind11.h>
// #include <pybind11/operators.h>
// #include "vector.h"  // Include the original struct definition

// namespace py = pybind11;

// PYBIND11_MODULE(Vectors, m) {
//     py::class_<rvector>(m, "rvector")
//         .def(py::init<>())  // Default constructor
//         .def(py::init<double, double, double>())  // Parameterized constructor
//         .def("__getitem__", [](const rvector &v, size_t i) {
//             if (i >= 3) throw py::index_error();
//             return v.col[i];
//         })
//         .def("__setitem__", [](rvector &v, size_t i, double value) {
//             if (i >= 3) throw py::index_error();
//             v.col[i] = value;
//         })
//         .def(py::self * double())  // Scalar multiplication
//         .def(py::self += py::self)  // In-place addition
//         .def(py::self -= py::self)  // In-place subtraction
//         .def(py::self *= double())  // In-place scalar multiplication
//         .def(-py::self)  // Negation
//         .def("__repr__", [](const rvector &v) {
//             return "rvector(" + std::to_string(v.col[0]) + ", " +
//                    std::to_string(v.col[1]) + ", " +
//                    std::to_string(v.col[2]) + ")";
//         });
// }
