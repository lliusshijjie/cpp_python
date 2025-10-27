#pragma once

#include <string>
#include <exception>
#include <functional>
#include <vector>
#include <memory>
#include <optional>
#include <map>
#include <typeindex>
#include <chrono>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace cpppy_bridge {

/**
 * @brief Base class for Python bridge exceptions.
 */
class PythonBridgeException : public std::exception {
public:
    explicit PythonBridgeException(const std::string& message);
    const char* what() const noexcept override;
    
protected:
    std::string m_message;
};

/**
 * @brief Exception for Python interpreter errors.
 */
class PythonInterpreterException : public PythonBridgeException {
public:
    explicit PythonInterpreterException(const std::string& message);
};

/**
 * @brief Exception for Python module loading errors.
 */
class PythonModuleException : public PythonBridgeException {
public:
    PythonModuleException(const std::string& module_name, const std::string& message);
    const std::string& getModuleName() const;
    
private:
    std::string m_module_name;
};

/**
 * @brief Exception for Python function call errors.
 */
class PythonFunctionException : public PythonBridgeException {
public:
    PythonFunctionException(const std::string& function_name, const std::string& message);
    const std::string& getFunctionName() const;
    
private:
    std::string m_function_name;
};

/**
 * @brief Exception for type conversion errors.
 */
class TypeConversionException : public PythonBridgeException {
public:
    TypeConversionException(const std::string& from_type, const std::string& to_type, const std::string& message);
    const std::string& getFromType() const;
    const std::string& getToType() const;
    
private:
    std::string m_from_type;
    std::string m_to_type;
};

/**
 * @brief Structure to hold Python error information.
 */
struct PythonErrorInfo {
    std::string type;           // Exception type
    std::string message;        // Error message
    std::string traceback;      // Stack trace
    std::string file;           // File name
    int line = -1;              // Line number
    std::string function;       // Function name
};

/**
 * @brief Error Handler Class
 * Responsible for catching, converting, and handling Python exceptions.
 */
class ErrorHandler {
public:
    // Type for error callback functions
    using ErrorCallback = std::function<void(const PythonErrorInfo&)>;
    
    // Set a global error callback
    static void setGlobalErrorCallback(ErrorCallback callback);
    
    // Add an error callback (supports multiple callbacks)
    static void addErrorCallback(ErrorCallback callback);
    
    // Clear all error callbacks
    static void clearErrorCallbacks();
    
    // Handle a Python exception
    static PythonErrorInfo handlePythonException(const py::error_already_set& e);
    
    // Convert a Python exception to a C++ exception
    static void convertPythonException(const py::error_already_set& e);
    
    // Safely execute Python code (with exception handling)
    template<typename Func>
    static auto safeExecute(Func&& func) -> decltype(func());
    
    // Safely execute Python code (returns an optional value)
    template<typename Func>
    static auto safeExecuteOptional(Func&& func) -> std::optional<decltype(func())>;
    
    // Get detailed information from a Python exception
    static PythonErrorInfo extractPythonErrorInfo(const py::error_already_set& e);
    
    // Format error information into a string
    static std::string formatErrorInfo(const PythonErrorInfo& info);
    
    // Log an error
    static void logError(const PythonErrorInfo& info);
    
    // Enable/disable verbose error messages
    static void setVerboseErrors(bool verbose);
    
    // Enable/disable error logging
    static void setErrorLogging(bool enable);
    
private:
    static std::vector<ErrorCallback> s_error_callbacks;
    static bool s_verbose_errors;
    static bool s_error_logging;
    
    // Trigger error callbacks
    static void triggerErrorCallbacks(const PythonErrorInfo& info);
    
    // Parse a Python traceback object
    static std::string parsePythonTraceback(const py::object& traceback_obj);
};

/**
 * @brief RAII-style Error Handler
 * Automatically handles exceptions within its scope.
 */
class ScopedErrorHandler {
public:
    explicit ScopedErrorHandler(ErrorHandler::ErrorCallback callback);
    ~ScopedErrorHandler();
    
    // Disable copy and move operations
    ScopedErrorHandler(const ScopedErrorHandler&) = delete;
    ScopedErrorHandler& operator=(const ScopedErrorHandler&) = delete;
    ScopedErrorHandler(ScopedErrorHandler&&) = delete;
    ScopedErrorHandler& operator=(ScopedErrorHandler&&) = delete;
    
private:
    ErrorHandler::ErrorCallback m_callback;
    bool m_registered = false;
};

/**
 * @brief Exception Converter
 * Uniformly converts different types of exceptions.
 */
class ExceptionConverter {
public:
    // Convert std::exception to a Python exception
    static void convertStdException(const std::exception& e);
    
    // Convert a Python exception to std::exception
    static std::unique_ptr<std::exception> convertPythonException(const py::error_already_set& e);
    
    // Register a custom exception converter
    template<typename ExceptionType>
    static void registerExceptionConverter(
        std::function<py::object(const ExceptionType&)> to_python,
        std::function<std::unique_ptr<ExceptionType>(const py::object&)> from_python
    );
    
private:
    // Map of exception converters
    static std::map<std::type_index, std::function<py::object(const std::exception&)>> s_to_python_converters;
    static std::map<std::string, std::function<std::unique_ptr<std::exception>(const py::object&)>> s_from_python_converters;
};

// Convenience macros
#define SAFE_PYTHON_CALL(call) \
    ErrorHandler::safeExecute([&]() { return call; })

#define SAFE_PYTHON_CALL_OPTIONAL(call) \
    ErrorHandler::safeExecuteOptional([&]() { return call; })

#define HANDLE_PYTHON_EXCEPTION(e) \
    do { \
        auto error_info = ErrorHandler::handlePythonException(e); \
        ErrorHandler::logError(error_info); \
        ErrorHandler::convertPythonException(e); \
    } while(0)

// Template method implementations
template<typename Func>
auto ErrorHandler::safeExecute(Func&& func) -> decltype(func()) {
    try {
        return func();
    } catch (const py::error_already_set& e) {
        auto error_info = handlePythonException(e);
        triggerErrorCallbacks(error_info);
        convertPythonException(e);
        throw; // This line won't be reached as convertPythonException throws
    } catch (const std::exception& e) {
        PythonErrorInfo info;
        info.type = "C++ Exception";
        info.message = e.what();
        triggerErrorCallbacks(info);
        throw;
    }
}

template<typename Func>
auto ErrorHandler::safeExecuteOptional(Func&& func) -> std::optional<decltype(func())> {
    try {
        return func();
    } catch (const py::error_already_set& e) {
        auto error_info = handlePythonException(e);
        triggerErrorCallbacks(error_info);
        return std::nullopt;
    } catch (const std::exception& e) {
        PythonErrorInfo info;
        info.type = "C++ Exception";
        info.message = e.what();
        triggerErrorCallbacks(info);
        return std::nullopt;
    }
}

template<typename ExceptionType>
void ExceptionConverter::registerExceptionConverter(
    std::function<py::object(const ExceptionType&)> to_python,
    std::function<std::unique_ptr<ExceptionType>(const py::object&)> from_python) {
    
    std::type_index type_idx(typeid(ExceptionType));
    
    s_to_python_converters[type_idx] = [to_python](const std::exception& e) -> py::object {
        const ExceptionType* typed_e = dynamic_cast<const ExceptionType*>(&e);
        if (typed_e) {
            return to_python(*typed_e);
        }
        return py::none();
    };
    
    std::string type_name = typeid(ExceptionType).name();
    s_from_python_converters[type_name] = [from_python](const py::object& obj) -> std::unique_ptr<std::exception> {
        auto typed_exception = from_python(obj);
        return std::unique_ptr<std::exception>(typed_exception.release());
    };
}

} // namespace cpppy_bridge