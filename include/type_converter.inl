#pragma once

#include "type_converter.h"
#include <iostream>
#include <typeinfo>
#include <typeindex>

namespace cpppy_bridge {

// TypeConverter 实现

template<typename T>
py::object TypeConverter::toPython(const T& value) {
    // 检查是否有自定义转换器
    if (CustomTypeRegistry::hasToPythonConverter<T>()) {
        return CustomTypeRegistry::convertToPython<T>(value);
    }
    
    // 使用pybind11的默认转换
    return py::cast(value);
}

template<typename T>
T TypeConverter::fromPython(const py::object& obj) {
    // 检查是否有自定义转换器
    if (CustomTypeRegistry::hasFromPythonConverter<T>()) {
        return CustomTypeRegistry::convertFromPython<T>(obj);
    }
    
    // 使用pybind11的默认转换
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

// ComplexTypeConverter 实现

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

// NumpyConverter 实现

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
    
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    
    // 检查矩阵是否为矩形
    for (const auto& row : matrix) {
        if (row.size() != cols) {
            throw std::runtime_error("Matrix must be rectangular for NumPy conversion");
        }
    }
    
    // 创建连续内存布局
    std::vector<T> flat_data;
    flat_data.reserve(rows * cols);
    
    for (const auto& row : matrix) {
        flat_data.insert(flat_data.end(), row.begin(), row.end());
    }
    
    py::array_t<T> result({rows, cols});
    auto buf = result.request();
    T* ptr = static_cast<T*>(buf.ptr);
    std::memcpy(ptr, flat_data.data(), flat_data.size() * sizeof(T));

    return result;
}

template<typename T>
std::vector<std::vector<T>> NumpyConverter::numpyToMatrix2D(const py::array_t<T>& arr) {
    auto buf = arr.request();
    
    if (buf.ndim != 2) {
        throw std::runtime_error("Expected 2D array for matrix conversion");
    }
    
    size_t rows = buf.shape[0];
    size_t cols = buf.shape[1];
    T* ptr = static_cast<T*>(buf.ptr);
    
    std::vector<std::vector<T>> result(rows, std::vector<T>(cols));
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = ptr[i * cols + j];
        }
    }
    
    return result;
}

// CustomTypeRegistry 实现

template<typename CppType>
void CustomTypeRegistry::registerToPython(std::function<py::object(const CppType&)> converter) {
    std::string type_name = typeid(CppType).name();
    s_to_python_converters[type_name] = [converter](const void* ptr) -> py::object {
        return converter(*static_cast<const CppType*>(ptr));
    };
}

template<typename CppType>
void CustomTypeRegistry::registerFromPython(std::function<CppType(const py::object&)> converter) {
    std::string type_name = typeid(CppType).name();
    s_from_python_converters[type_name] = [converter](const py::object& obj) -> void* {
        return new CppType(converter(obj));
    };
}

template<typename CppType>
bool CustomTypeRegistry::hasToPythonConverter() {
    std::string type_name = typeid(CppType).name();
    return s_to_python_converters.find(type_name) != s_to_python_converters.end();
}

template<typename CppType>
bool CustomTypeRegistry::hasFromPythonConverter() {
    std::string type_name = typeid(CppType).name();
    return s_from_python_converters.find(type_name) != s_from_python_converters.end();
}

template<typename CppType>
py::object CustomTypeRegistry::convertToPython(const CppType& value) {
    std::string type_name = typeid(CppType).name();
    auto it = s_to_python_converters.find(type_name);
    if (it != s_to_python_converters.end()) {
        return it->second(&value);
    }
    throw std::runtime_error("No registered converter for type: " + type_name);
}

template<typename CppType>
CppType CustomTypeRegistry::convertFromPython(const py::object& obj) {
    std::string type_name = typeid(CppType).name();
    auto it = s_from_python_converters.find(type_name);
    if (it != s_from_python_converters.end()) {
        void* ptr = it->second(obj);
        CppType result = *static_cast<CppType*>(ptr);
        delete static_cast<CppType*>(ptr);
        return result;
    }
    throw std::runtime_error("No registered converter for type: " + type_name);
}

} // namespace cpppy_bridge
