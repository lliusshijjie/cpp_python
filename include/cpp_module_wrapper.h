#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <any>

namespace cpppy_bridge {

// Forward declarations
class CppModuleWrapper;
class CppFunctionBinder;
class CppClassBinder;

/**
 * @brief C++ Module Wrapper - Exposes C++ code to Python.
 *
 * This class manages Python bindings for C++ functions and classes,
 * allowing Python code to directly call C++ functionality.
 */
class CppModuleWrapper {
public:
    /**
     * @brief Constructor
     * @param module_name The name of the Python module.
     * @param description A description of the module.
     */
    CppModuleWrapper(const std::string& module_name, const std::string& description = "");
    
    /**
     * @brief Destructor
     */
    ~CppModuleWrapper();

    /**
     * @brief Binds a C++ function to the Python module.
     * @tparam Func The function type.
     * @param name The name of the function in Python.
     * @param func A C++ function pointer or lambda.
     * @param docstring The function's docstring.
     * @return A reference to itself for chaining calls.
     */
    template<typename Func>
    CppModuleWrapper& bind_function(const std::string& name, Func&& func, 
                                   const std::string& docstring = "");

    /**
     * @brief Binds a C++ class to the Python module.
     * @tparam Class The class type.
     * @param name The name of the class in Python.
     * @param docstring The class's docstring.
     * @return A class binder for further configuration.
     */
    template<typename Class>
    CppClassBinder<Class> bind_class(const std::string& name, 
                                    const std::string& docstring = "");

    /**
     * @brief Binds a constant to the Python module.
     * @tparam T The constant type.
     * @param name The name of the constant in Python.
     * @param value The value of the constant.
     * @return A reference to itself for chaining calls.
     */
    template<typename T>
    CppModuleWrapper& bind_constant(const std::string& name, const T& value);

    /**
     * @brief Creates the Python extension module.
     * @return A pybind11 module object.
     */
    pybind11::module_ create_module();

    /**
     * @brief Gets the module name.
     */
    const std::string& get_module_name() const { return module_name_; }

    /**
     * @brief Registers the module with the Python interpreter.
     * @param force_reload Whether to force a reload.
     */
    void register_module(bool force_reload = false);

private:
    std::string module_name_;
    std::string description_;
    std::unique_ptr<pybind11::module_> module_;
    std::map<std::string, std::any> bound_functions_;
    std::map<std::string, std::any> bound_classes_;
    std::map<std::string, std::any> bound_constants_;
};

/**
 * @brief C++ Function Binder
 *
 * Provides finer-grained control over function binding.
 */
class CppFunctionBinder {
public:
    CppFunctionBinder();
    ~CppFunctionBinder();
    
    template<typename Func>
    CppFunctionBinder(const std::string& name, Func&& func);

    /**
     * @brief Sets the function's documentation.
     */
    CppFunctionBinder& doc(const std::string& docstring);

    /**
     * @brief Sets the parameter names.
     */
    CppFunctionBinder& arg_names(const std::vector<std::string>& names);

    /**
     * @brief Sets the default argument values.
     */
    template<typename... Args>
    CppFunctionBinder& default_args(Args&&... args);

private:
    std::string name_;
    std::string docstring_;
    std::vector<std::string> arg_names_;
    std::any function_;
};

/**
 * @brief C++ Class Binder
 *
 * Used to bind C++ class constructors, methods, properties, etc.
 */
template<typename Class>
class CppClassBinder {
public:
    CppClassBinder(pybind11::class_<Class>& py_class);

    /**
     * @brief Binds a constructor.
     */
    template<typename... Args>
    CppClassBinder& constructor();

    /**
     * @brief Binds a member function.
     */
    template<typename Func>
    CppClassBinder& method(const std::string& name, Func&& func, 
                          const std::string& docstring = "");

    /**
     * @brief Binds a static method.
     */
    template<typename Func>
    CppClassBinder& static_method(const std::string& name, Func&& func,
                                 const std::string& docstring = "");

    /**
     * @brief Binds a property.
     */
    template<typename T>
    CppClassBinder& property(const std::string& name, T Class::* member);

    /**
     * @brief Binds a read-only property.
     */
    template<typename Getter>
    CppClassBinder& property_readonly(const std::string& name, Getter&& getter);

    /**
     * @brief Binds a read-write property.
     */
    template<typename Getter, typename Setter>
    CppClassBinder& property(const std::string& name, Getter&& getter, Setter&& setter);

private:
    pybind11::class_<Class>& py_class_;
};

/**
 * @brief Module Registry - Simplifies the module registration process.
 */
class ModuleRegistry {
public:
    /**
     * @brief Gets the singleton instance.
     */
    static ModuleRegistry& getInstance();

    /**
     * @brief Registers a module.
     */
    void registerModule(CppModuleWrapper& wrapper);

    /**
     * @brief Retrieves a module by name.
     */
    CppModuleWrapper* getModule(const std::string& name);

private:
    ModuleRegistry() = default;
    std::map<std::string, CppModuleWrapper*> modules_;
};

#include "cpp_module_wrapper.inl"

} // namespace cpppy_bridge