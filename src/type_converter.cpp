#include "type_converter.h"
#include <iostream>
#include <typeinfo>
#include <typeindex>

namespace cpppy_bridge {

// TypeConverter non-template implementations
std::string TypeConverter::getPythonType(const py::object& obj) {
    return py::str(obj.get_type()).cast<std::string>();
}

bool TypeConverter::isPythonNone(const py::object& obj) {
    return obj.is_none();
}

bool TypeConverter::isPythonBool(const py::object& obj) {
    return py::isinstance<py::bool_>(obj);
}

bool TypeConverter::isPythonInt(const py::object& obj) {
    return py::isinstance<py::int_>(obj);
}

bool TypeConverter::isPythonFloat(const py::object& obj) {
    return py::isinstance<py::float_>(obj);
}

bool TypeConverter::isPythonString(const py::object& obj) {
    return py::isinstance<py::str>(obj);
}

bool TypeConverter::isPythonList(const py::object& obj) {
    return py::isinstance<py::list>(obj);
}

bool TypeConverter::isPythonDict(const py::object& obj) {
    return py::isinstance<py::dict>(obj);
}

bool TypeConverter::isPythonTuple(const py::object& obj) {
    return py::isinstance<py::tuple>(obj);
}

bool TypeConverter::isPythonSet(const py::object& obj) {
    return py::isinstance<py::set>(obj);
}

// Basic type specializations
template<>
py::object TypeConverter::toPython<std::string>(const std::string& value) {
    return py::str(value);
}

template<>
py::object TypeConverter::toPython<int>(const int& value) {
    return py::int_(value);
}

template<>
py::object TypeConverter::toPython<double>(const double& value) {
    return py::float_(value);
}

template<>
py::object TypeConverter::toPython<bool>(const bool& value) {
    return py::bool_(value);
}

template<>
py::object TypeConverter::toPython<float>(const float& value) {
    return py::float_(value);
}

template<>
py::object TypeConverter::toPython<long>(const long& value) {
    return py::int_(value);
}

template<>
std::string TypeConverter::fromPython<std::string>(const py::object& obj) {
    if (isPythonString(obj)) {
        return obj.cast<std::string>();
    }
    throw std::runtime_error("Cannot convert Python object to std::string");
}

template<>
int TypeConverter::fromPython<int>(const py::object& obj) {
    if (isPythonInt(obj)) {
        return obj.cast<int>();
    }
    throw std::runtime_error("Cannot convert Python object to int");
}

template<>
double TypeConverter::fromPython<double>(const py::object& obj) {
    if (isPythonFloat(obj) || isPythonInt(obj)) {
        return obj.cast<double>();
    }
    throw std::runtime_error("Cannot convert Python object to double");
}

template<>
bool TypeConverter::fromPython<bool>(const py::object& obj) {
    if (isPythonBool(obj)) {
        return obj.cast<bool>();
    }
    throw std::runtime_error("Cannot convert Python object to bool");
}

template<>
float TypeConverter::fromPython<float>(const py::object& obj) {
    if (isPythonFloat(obj) || isPythonInt(obj)) {
        return obj.cast<float>();
    }
    throw std::runtime_error("Cannot convert Python object to float");
}

template<>
long TypeConverter::fromPython<long>(const py::object& obj) {
    if (isPythonInt(obj)) {
        return obj.cast<long>();
    }
    throw std::runtime_error("Cannot convert Python object to long");
}

// NumpyConverter non-template implementations
std::vector<size_t> NumpyConverter::getArrayShape(const py::array& arr) {
    auto buf = arr.request();
    std::vector<size_t> shape;
    for (py::ssize_t dim : buf.shape) {
        shape.push_back(static_cast<size_t>(dim));
    }
    return shape;
}

std::string NumpyConverter::getArrayDtype(const py::array& arr) {
    return py::str(arr.dtype()).cast<std::string>();
}

size_t NumpyConverter::getArraySize(const py::array& arr) {
    return static_cast<size_t>(arr.size());
}

// CustomTypeRegistry static member initialization
std::unordered_map<std::string, std::function<py::object(const void*)>>
    CustomTypeRegistry::s_to_python_converters;

std::unordered_map<std::string, std::function<void*(const py::object&)>>
    CustomTypeRegistry::s_from_python_converters;

} // namespace cpppy_bridge