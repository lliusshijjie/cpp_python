#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "error_handler.h"

namespace py = pybind11;

namespace cpppy_bridge {

/**
 * @brief Python Interpreter Manager
 * Manages the initialization, finalization, and lifecycle of the Python interpreter.
 */
class PythonInterpreter {
public:
    static PythonInterpreter& getInstance();
    
    void initialize();
    void finalize();
    bool isInitialized() const;
    
    // Add a search path for Python modules
    void addModulePath(const std::string& path);
    
    // Execute Python code
    py::object execute(const std::string& code);
    
private:
    PythonInterpreter() = default;
    ~PythonInterpreter();
    
    bool m_initialized = false;
    std::unique_ptr<py::scoped_interpreter> m_interpreter;
};

/**
 * @brief Python Module Wrapper
 * Provides a high-level interface for loading and calling Python modules.
 */
class PythonModule {
public:
    explicit PythonModule(const std::string& module_name);
    ~PythonModule() = default;
    
    // Check if the module was loaded successfully
    bool isLoaded() const;
    
    // Check if a function exists in the module
    bool hasFunction(const std::string& func_name) const;
    
    // Call a Python function (template method for various argument types)
    template<typename ReturnType, typename... Args>
    ReturnType callFunction(const std::string& func_name, Args&&... args);
    
    // Call a Python function (returns py::object)
    py::object callFunction(const std::string& func_name, const std::vector<py::object>& args = {});
    
    // Get a module attribute
    py::object getAttribute(const std::string& attr_name);
    
    // Set a module attribute
    void setAttribute(const std::string& attr_name, const py::object& value);
    
private:
    std::string m_module_name;
    py::module m_module;
    bool m_loaded = false;
};

/**
 * @brief Python Function Wrapper
 * Provides a more convenient interface for calling functions.
 */
class PythonFunction {
public:
    PythonFunction(const std::string& module_name, const std::string& func_name);
    PythonFunction(std::shared_ptr<PythonModule> module, const std::string& func_name);
    
    // Check if the function is valid and usable
    bool isValid() const;
    
    // Call the function
    template<typename ReturnType, typename... Args>
    ReturnType operator()(Args&&... args);
    
    template<typename ReturnType, typename... Args>
    ReturnType call(Args&&... args);

    py::object callPy(const std::vector<py::object>& args = {});
    
private:
    std::shared_ptr<PythonModule> m_module;
    std::string m_func_name;
    py::object m_function;
    bool m_valid = false;
};

/**
 * @brief Bulk Python Operation Manager
 * Manages calls to multiple Python modules and functions.
 */
class PythonBridge {
public:
    PythonBridge();
    ~PythonBridge();
    
    // Initialize the bridge
    bool initialize(const std::vector<std::string>& module_paths = {});
    
    // Load a Python module
    std::shared_ptr<PythonModule> loadModule(const std::string& module_name);
    
    // Create a function wrapper
    std::shared_ptr<PythonFunction> createFunction(const std::string& module_name, 
                                                   const std::string& func_name);
    
    // Execute a Python script file
    py::object executeFile(const std::string& file_path);
    
    // Execute a string of Python code
    py::object executeCode(const std::string& code);
    
    // Get a previously loaded module
    std::shared_ptr<PythonModule> getModule(const std::string& module_name);
    
private:
    std::map<std::string, std::shared_ptr<PythonModule>> m_modules;
    bool m_initialized = false;
};

// Template method implementations
template<typename ReturnType, typename... Args>
ReturnType PythonModule::callFunction(const std::string& func_name, Args&&... args) {
    if (!m_loaded) {
        throw PythonModuleException(m_module_name, "Module not loaded");
    }
    
    try {
        py::object func = m_module.attr(func_name.c_str());
        py::object result = func(std::forward<Args>(args)...);
        if constexpr (std::is_void_v<ReturnType>) {
            return;
        } else {
            return result.cast<ReturnType>();
        }
    } catch (const py::error_already_set& e) {
        ErrorHandler::handlePythonException(e);
        ErrorHandler::convertPythonException(e);
        throw; // Should not be reached
    }
}

template<typename ReturnType, typename... Args>
ReturnType PythonFunction::operator()(Args&&... args) {
    if (!m_valid) {
        throw PythonFunctionException(m_func_name, "Invalid function");
    }
    
    try {
        py::object result = m_function(std::forward<Args>(args)...);
        if constexpr (std::is_void_v<ReturnType>) {
            return;
        } else {
            return result.cast<ReturnType>();
        }
    } catch (const py::error_already_set& e) {
        ErrorHandler::handlePythonException(e);
        ErrorHandler::convertPythonException(e);
        throw; // Should not be reached
    }
}

template<typename ReturnType, typename... Args>
ReturnType PythonFunction::call(Args&&... args) {
    if (!m_valid) {
        throw PythonFunctionException(m_func_name, "Invalid function");
    }
    
    try {
        py::object result = m_function(std::forward<Args>(args)...);
        if constexpr (std::is_void_v<ReturnType>) {
            return;
        } else {
            return result.cast<ReturnType>();
        }
    } catch (const py::error_already_set& e) {
        ErrorHandler::handlePythonException(e);
        ErrorHandler::convertPythonException(e);
        throw; // Should not be reached
    }
}

} // namespace cpppy_bridge