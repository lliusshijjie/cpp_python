#include "cpp_library.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace example_lib;

PYBIND11_MODULE(example_cpp_lib_module, m) {
    m.doc() = "Example C++ library with Python bindings";

    py::class_<MathCalculator>(m, "MathCalculator")
        .def(py::init<>())
        .def("add", &MathCalculator::add)
        .def("subtract", &MathCalculator::subtract)
        .def("multiply", &MathCalculator::multiply)
        .def("divide", &MathCalculator::divide);

    py::class_<StringProcessor>(m, "StringProcessor")
        .def(py::init<>())
        .def("concatenate", &StringProcessor::concatenate)
        .def("get_length", &StringProcessor::get_length);

    py::class_<DataContainer>(m, "DataContainer")
        .def(py::init<>())
        .def("add_data", &DataContainer::add_data)
        .def("get_data", &DataContainer::get_data)
        .def("get_all_data", &DataContainer::get_all_data);

    py::class_<CallbackExample>(m, "CallbackExample")
        .def(py::init<>())
        .def("execute_callback", &CallbackExample::execute_callback);

    m.def("global_function_example", &global_function_example, "An example of a global function");
}