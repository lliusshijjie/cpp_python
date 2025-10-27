#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <tuple>
#include <optional>
#include <variant>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace cpppy_bridge {

/**
 * @brief Type Converter Class
 * Provides functionality for converting data types between C++ and Python.
 */
class TypeConverter {
public:
    // C++ -> Python conversion
    template<typename T>
    static py::object toPython(const T& value);
    
    // Python -> C++ conversion
    template<typename T>
    static T fromPython(const py::object& obj);
    
    // Check if a Python object can be converted to a specific C++ type
    template<typename T>
    static bool canConvert(const py::object& obj);
    
    // Get type information of a Python object
    static std::string getPythonType(const py::object& obj);
    
    // Safe conversion (with exception handling)
    template<typename T>
    static std::optional<T> safeCast(const py::object& obj);
    
private:
    // Internal helper methods
    static bool isPythonNone(const py::object& obj);
    static bool isPythonBool(const py::object& obj);
    static bool isPythonInt(const py::object& obj);
    static bool isPythonFloat(const py::object& obj);
    static bool isPythonString(const py::object& obj);
    static bool isPythonList(const py::object& obj);
    static bool isPythonDict(const py::object& obj);
    static bool isPythonTuple(const py::object& obj);
    static bool isPythonSet(const py::object& obj);
};

/**
 * @brief Complex Data Structure Converter
 * Handles conversion of nested containers and custom data structures.
 */
class ComplexTypeConverter {
public:
    // Nested container conversion
    template<typename T>
    static py::object vectorToPython(const std::vector<T>& vec);
    
    template<typename T>
    static std::vector<T> vectorFromPython(const py::object& obj);
    
    template<typename K, typename V>
    static py::object mapToPython(const std::map<K, V>& map);
    
    template<typename K, typename V>
    static std::map<K, V> mapFromPython(const py::object& obj);
    
    // Tuple conversion
    template<typename... Args>
    static py::object tupleToPython(const std::tuple<Args...>& tuple);
    
    template<typename... Args>
    static std::tuple<Args...> tupleFromPython(const py::object& obj);
    
    // Optional type conversion
    template<typename T>
    static py::object optionalToPython(const std::optional<T>& opt);
    
    template<typename T>
    static std::optional<T> optionalFromPython(const py::object& obj);
    
    // Variant type conversion
    template<typename... Types>
    static py::object variantToPython(const std::variant<Types...>& var);
    
    template<typename... Types>
    static std::variant<Types...> variantFromPython(const py::object& obj);
};

/**
 * @brief NumPy Array Converter
 * Handles conversions between NumPy arrays and C++ arrays/vectors.
 */
class NumpyConverter {
public:
    // 1D array conversion
    template<typename T>
    static py::array_t<T> vectorToNumpy(const std::vector<T>& vec);
    
    template<typename T>
    static std::vector<T> numpyToVector(const py::array_t<T>& arr);
    
    // 2D array conversion
    template<typename T>
    static py::array_t<T> matrix2DToNumpy(const std::vector<std::vector<T>>& matrix);
    
    template<typename T>
    static std::vector<std::vector<T>> numpyToMatrix2D(const py::array_t<T>& arr);
    
    // Get general array information
    static std::vector<size_t> getArrayShape(const py::array& arr);
    static std::string getArrayDtype(const py::array& arr);
    static size_t getArraySize(const py::array& arr);
};

/**
 * @brief Custom Type Registry
 * Allows users to register custom type conversion functions.
 */
class CustomTypeRegistry {
public:
    // Register custom conversion functions
    template<typename CppType>
    static void registerToPython(std::function<py::object(const CppType&)> converter);
    
    template<typename CppType>
    static void registerFromPython(std::function<CppType(const py::object&)> converter);
    
    // Check if a converter is registered
    template<typename CppType>
    static bool hasToPythonConverter();
    
    template<typename CppType>
    static bool hasFromPythonConverter();
    
    // Use a registered converter
    template<typename CppType>
    static py::object convertToPython(const CppType& value);
    
    template<typename CppType>
    static CppType convertFromPython(const py::object& obj);
    
private:
    static std::unordered_map<std::string, std::function<py::object(const void*)>> s_to_python_converters;
    static std::unordered_map<std::string, std::function<void*(const py::object&)>> s_from_python_converters;
};

// Convenience macro
#define REGISTER_CUSTOM_TYPE_CONVERTER(CppType, ToPythonFunc, FromPythonFunc) \
    do { \
        CustomTypeRegistry::registerToPython<CppType>(ToPythonFunc); \
        CustomTypeRegistry::registerFromPython<CppType>(FromPythonFunc); \
    } while(0)

// Template specialization declarations
template<> py::object TypeConverter::toPython<std::string>(const std::string& value);
template<> py::object TypeConverter::toPython<int>(const int& value);
template<> py::object TypeConverter::toPython<double>(const double& value);
template<> py::object TypeConverter::toPython<bool>(const bool& value);
template<> py::object TypeConverter::toPython<float>(const float& value);
template<> py::object TypeConverter::toPython<long>(const long& value);

template<> std::string TypeConverter::fromPython<std::string>(const py::object& obj);
template<> int TypeConverter::fromPython<int>(const py::object& obj);
template<> double TypeConverter::fromPython<double>(const py::object& obj);
template<> bool TypeConverter::fromPython<bool>(const py::object& obj);
template<> float TypeConverter::fromPython<float>(const py::object& obj);
template<> long TypeConverter::fromPython<long>(const py::object& obj);

} // namespace cpppy_bridge

#include "type_converter.inl"