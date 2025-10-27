#pragma once

#include "type_converter.h"
#include <iostream>
#include <typeinfo>
#include <typeindex>

namespace cpppy_bridge {

// TypeConverter implementation

template<typename T>
py::object TypeConverter::toPython(const T& value) {
    // Check for a custom converter
    if (CustomTypeRegistry::hasToPythonConverter<T>()) {
        return CustomTypeRegistry::convertToPython<T>(value);
    }
    
    // Use pybind11's default conversion
    return py::cast(value);
}

template<typename T>
T TypeConverter::fromPython(const py::object& obj) {
    // Check for a custom converter
    if (CustomTypeRegistry::hasFromPythonConverter<T>()) {
        return CustomTypeRegistry::convertFromPython<T>(obj);
    }
    
    // Use pybind11's default conversion
    return obj.cast<T>();
}

template<typename T>
bool TypeConverter::canConvert(const py::object& obj) {
    try {
        obj.cast<T>();
        return true;
    } catch (const py::cast_error&) {
        return false;
    }
}

template<typename T>
std::optional<T> TypeConverter::safeCast(const py::object& obj) {
    try {
        return fromPython<T>(obj);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

// ComplexTypeConverter implementation

template<typename T>
py::object ComplexTypeConverter::vectorToPython(const std::vector<T>& vec) {
    py::list result;
    for (const auto& item : vec) {
        result.append(TypeConverter::toPython(item));
    }
    return result;
}

template<typename T>
std::vector<T> ComplexTypeConverter::vectorFromPython(const py::object& obj) {
    if (!TypeConverter::isPythonList(obj)) {
        throw std::runtime_error("Expected Python list for vector conversion");
    }
    
    py::list py_list = obj.cast<py::list>();
    std::vector<T> result;
    result.reserve(py_list.size());
    
    for (const auto& item : py_list) {
        result.push_back(TypeConverter::fromPython<T>(item));
    }
    
    return result;
}

template<typename K, typename V>
py::object ComplexTypeConverter::mapToPython(const std::map<K, V>& map) {
    py::dict result;
    for (const auto& [key, value] : map) {
        result[TypeConverter::toPython(key)] = TypeConverter::toPython(value);
    }
    return result;
}

template<typename K, typename V>
std::map<K, V> ComplexTypeConverter::mapFromPython(const py::object& obj) {
    if (!TypeConverter::isPythonDict(obj)) {
        throw std::runtime_error("Expected Python dict for map conversion");
    }
    
    py::dict py_dict = obj.cast<py::dict>();
    std::map<K, V> result;
    
    for (const auto& item : py_dict) {
        K key = TypeConverter::fromPython<K>(item.first);
        V value = TypeConverter::fromPython<V>(item.second);
        result[key] = value;
    }
    
    return result;
}


template<typename... Args>
py::object ComplexTypeConverter::tupleToPython(const std::tuple<Args...>& tuple)
{
    py::tuple result(sizeof...(Args));
    std::apply([&](const Args&... args) {
        size_t i = 0;
        ((result[i++] = TypeConverter::toPython(args)), ...);
    }, tuple);
    return result;
}


template<typename... Args>
std::tuple<Args...> ComplexTypeConverter::tupleFromPython(const py::object& obj)
{
    if (!py::isinstance<py::tuple>(obj)) {
        throw std::runtime_error("Expected a Python tuple");
    }
    py::tuple py_tuple = obj.cast<py::tuple>();
    if (py_tuple.size() != sizeof...(Args)) {
        throw std::runtime_error("Tuple size mismatch");
    }
    return py_tuple.cast<std::tuple<Args...>>();
}


template<typename T>
py::object ComplexTypeConverter::optionalToPython(const std::optional<T>& opt)
{
    if (opt.has_value()) {
        return TypeConverter::toPython(opt.value());
    }
    return py::none();
}


template<typename T>
std::optional<T> ComplexTypeConverter::optionalFromPython(const py::object& obj)
{
    if (obj.is_none()) {
        return std::nullopt;
    }
    return TypeConverter::fromPython<T>(obj);
}


template<typename... Types>
py::object ComplexTypeConverter::variantToPython(const std::variant<Types...>& var)
{
    return std::visit([](const auto& value) {
        return TypeConverter::toPython(value);
    }, var);
}

template<typename... Types>
std::variant<Types...> ComplexTypeConverter::variantFromPython(const py::object& obj)
{
    // This is a simplified implementation. A real implementation would need to
    // try converting to each type in Types...
    // This requires more advanced template metaprogramming.
    // For now, we will just throw an error.
    throw std::runtime_error("variantFromPython is not fully implemented");
}

// NumpyConverter implementation

template<typename T>
py::array_t<T> NumpyConverter::vectorToNumpy(const std::vector<T>& vec) {
    return py::array_t<T>(
        vec.size(),                           // shape
        vec.data()                           // data pointer
    );
}

template<typename T>
std::vector<T> NumpyConverter::numpyToVector(const py::array_t<T>& arr) {
    auto buf = arr.request();
    
    if (buf.ndim != 1) {
        throw std::runtime_error("Expected 1D array for vector conversion");
    }
    
    T* ptr = static_cast<T*>(buf.ptr);
    return std::vector<T>(ptr, ptr + buf.shape[0]);
}

template<typename T>
py::array_t<T> NumpyConverter::matrix2DToNumpy(const std::vector<std::vector<T>>& matrix) {
    if (matrix.empty()) {
        return py::array_t<T>();
    }
    
    const size_t rows = matrix.size();
    const size_t cols = matrix[0].size();
    
    py::array_t<T> arr({rows, cols});
    auto buf = arr.request();
    
    T* ptr = static_cast<T*>(buf.ptr);
    
    for (size_t i = 0; i < rows; ++i) {
        if (matrix[i].size() != cols) {
            throw std::runtime_error("Inconsistent number of columns in 2D matrix");
        }
        std::copy(matrix[i].begin(), matrix[i].end(), ptr + i * cols);
    }
    
    return arr;
}

template<typename T>
std::vector<std::vector<T>> NumpyConverter::numpyToMatrix2D(const py::array_t<T>& arr) {
    auto buf = arr.request();
    
    if (buf.ndim != 2) {
        throw std::runtime_error("Expected 2D array for matrix conversion");
    }
    
    const size_t rows = buf.shape[0];
    const size_t cols = buf.shape[1];
    
    std::vector<std::vector<T>> result(rows, std::vector<T>(cols));
    T* ptr = static_cast<T*>(buf.ptr);
    
    for (size_t i = 0; i < rows; ++i) {
        std::copy(ptr + i * cols, ptr + (i + 1) * cols, result[i].begin());
    }
    
    return result;
}

// CustomTypeRegistry implementation

template<typename CppType>
void CustomTypeRegistry::registerToPython(std::function<py::object(const CppType&)> converter) {
    s_to_python_converters[typeid(CppType).name()] = 
        [converter](const void* value) -> py::object {
            return converter(*static_cast<const CppType*>(value));
        };
}

template<typename CppType>
void CustomTypeRegistry::registerFromPython(std::function<CppType(const py::object&)> converter) {
    s_from_python_converters[typeid(CppType).name()] = 
        [converter](const py::object& obj) -> void* {
            return new CppType(converter(obj));
        };
}

template<typename CppType>
bool CustomTypeRegistry::hasToPythonConverter() {
    return s_to_python_converters.count(typeid(CppType).name()) > 0;
}

template<typename CppType>
bool CustomTypeRegistry::hasFromPythonConverter() {
    return s_from_python_converters.count(typeid(CppType).name()) > 0;
}

template<typename CppType>
py::object CustomTypeRegistry::convertToPython(const CppType& value) {
    auto it = s_to_python_converters.find(typeid(CppType).name());
    if (it == s_to_python_converters.end()) {
        throw std::runtime_error("No to-Python converter registered for this type");
    }
    return it->second(&value);
}

template<typename CppType>
CppType CustomTypeRegistry::convertFromPython(const py::object& obj) {
    auto it = s_from_python_converters.find(typeid(CppType).name());
    if (it == s_from_python_converters.end()) {
        throw std::runtime_error("No from-Python converter registered for this type");
    }
    void* ptr = it->second(obj);
    CppType result = *static_cast<CppType*>(ptr);
    delete static_cast<CppType*>(ptr);
    return result;
}

} // namespace cpppy_bridge
