#include "error_handler.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace cpppy_bridge {

// PythonBridgeException 实现
PythonBridgeException::PythonBridgeException(const std::string &message)
    : m_message(message) {}

const char *PythonBridgeException::what() const noexcept {
    return m_message.c_str();
}

// PythonInterpreterException 实现
PythonInterpreterException::PythonInterpreterException(const std::string &message)
    : PythonBridgeException("Python Interpreter Error: " + message) {}

// PythonModuleException 实现
PythonModuleException::PythonModuleException(const std::string &module_name, const std::string &message)
    : PythonBridgeException("Python Module Error [" + module_name + "]: " + message), m_module_name(module_name) {}

const std::string &PythonModuleException::getModuleName() const {
    return m_module_name;
}

// PythonFunctionException 实现
PythonFunctionException::PythonFunctionException(const std::string &function_name, const std::string &message)
    : PythonBridgeException("Python Function Error [" + function_name + "]: " + message), m_function_name(function_name) {}

const std::string &PythonFunctionException::getFunctionName() const {
    return m_function_name;
}

// TypeConversionException 实现
TypeConversionException::TypeConversionException(const std::string &from_type, const std::string &to_type, const std::string &message)
    : PythonBridgeException("Type Conversion Error [" + from_type + " -> " + to_type + "]: " + message), m_from_type(from_type), m_to_type(to_type) {}

const std::string &TypeConversionException::getFromType() const {
    return m_from_type;
}

const std::string &TypeConversionException::getToType() const {
    return m_to_type;
}

// ErrorHandler 静态成员初始化
std::vector<ErrorHandler::ErrorCallback> ErrorHandler::s_error_callbacks;
bool ErrorHandler::s_verbose_errors = true;
bool ErrorHandler::s_error_logging = true;

void ErrorHandler::setGlobalErrorCallback(ErrorCallback callback) {
    s_error_callbacks.clear();
    s_error_callbacks.push_back(callback);
}

void ErrorHandler::addErrorCallback(ErrorCallback callback) {
    s_error_callbacks.push_back(callback);
}

void ErrorHandler::clearErrorCallbacks() {
    s_error_callbacks.clear();
}

PythonErrorInfo ErrorHandler::handlePythonException(const py::error_already_set &e) {
    auto error_info = extractPythonErrorInfo(e);

    if (s_error_logging) {
        logError(error_info);
    }

    triggerErrorCallbacks(error_info);

    return error_info;
}

void ErrorHandler::convertPythonException(const py::error_already_set &e) {
    auto error_info = extractPythonErrorInfo(e);

    // 根据Python异常类型转换为相应的C++异常
    if (error_info.type == "ModuleNotFoundError" || error_info.type == "ImportError") {
        throw PythonModuleException("unknown", error_info.message);
    } else if (error_info.type == "AttributeError") {
        throw PythonFunctionException("unknown", error_info.message);
    } else if (error_info.type == "TypeError" || error_info.type == "ValueError") {
        throw TypeConversionException("unknown", "unknown", error_info.message);
    } else {
        throw PythonBridgeException(formatErrorInfo(error_info));
    }
}

PythonErrorInfo ErrorHandler::extractPythonErrorInfo(const py::error_already_set &e) {
    PythonErrorInfo info;

    try {
        // 获取异常类型
        if (e.type()) {
            py::object type_obj = py::reinterpret_borrow<py::object>(e.type());
            if (py::hasattr(type_obj, "__name__")) {
                info.type = py::str(type_obj.attr("__name__")).cast<std::string>();
            } else {
                info.type = "Unknown";
            }
        }

        // 获取异常消息
        if (e.value()) {
            py::object value_obj = py::reinterpret_borrow<py::object>(e.value());
            info.message = py::str(value_obj).cast<std::string>();
        }

        // 获取堆栈跟踪
        if (e.trace()) {
            py::object trace_obj = py::reinterpret_borrow<py::object>(e.trace());
            info.traceback = parsePythonTraceback(trace_obj);

            // 尝试从堆栈跟踪中提取文件名和行号
            try {
                py::module traceback_module = py::module::import("traceback");
                py::object tb_list = traceback_module.attr("extract_tb")(trace_obj);

                if (py::len(tb_list) > 0) {
                    py::object last_frame = tb_list[py::cast(py::len(tb_list) - 1)];
                    if (py::hasattr(last_frame, "filename")) {
                        info.file = py::str(last_frame.attr("filename")).cast<std::string>();
                    }
                    if (py::hasattr(last_frame, "lineno")) {
                        info.line = py::int_(last_frame.attr("lineno")).cast<int>();
                    }
                    if (py::hasattr(last_frame, "name")) {
                        info.function = py::str(last_frame.attr("name")).cast<std::string>();
                    }
                }
            } catch (...) {
                // 忽略堆栈跟踪解析错误
            }
        }
    } catch (...) {
        // 如果解析异常信息失败，使用默认值
        info.type = "Unknown";
        info.message = "Failed to extract Python error information";
    }

    return info;
}

std::string ErrorHandler::formatErrorInfo(const PythonErrorInfo &info) {
    std::ostringstream oss;

    oss << "Python Error: " << info.type;
    if (!info.message.empty()) {
        oss << " - " << info.message;
    }

    if (s_verbose_errors) {
        if (!info.file.empty()) {
            oss << "\n  File: " << info.file;
            if (info.line > 0) {
                oss << ":" << info.line;
            }
        }

        if (!info.function.empty()) {
            oss << "\n  Function: " << info.function;
        }

        if (!info.traceback.empty()) {
            oss << "\n  Traceback:\n"
                << info.traceback;
        }
    }

    return oss.str();
}

void ErrorHandler::logError(const PythonErrorInfo &info) {
    if (!s_error_logging) {
        return;
    }

    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;

#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif

    std::ostringstream log_stream;
    log_stream << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "] ";
    log_stream << formatErrorInfo(info);

    std::cerr << log_stream.str() << std::endl;
}

void ErrorHandler::setVerboseErrors(bool verbose) {
    s_verbose_errors = verbose;
}

void ErrorHandler::setErrorLogging(bool enable) {
    s_error_logging = enable;
}

void ErrorHandler::triggerErrorCallbacks(const PythonErrorInfo &info) {
    for (const auto &callback : s_error_callbacks) {
        try {
            callback(info);
        } catch (...) {
            // 忽略回调函数中的异常，避免无限递归
            std::cerr << "Error in error callback function" << std::endl;
        }
    }
}

std::string ErrorHandler::parsePythonTraceback(const py::object &traceback_obj) {
    try {
        py::module traceback_module = py::module::import("traceback");
        py::object formatted = traceback_module.attr("format_tb")(traceback_obj);

        std::ostringstream oss;
        for (const auto &line : formatted) {
            oss << py::str(line).cast<std::string>();
        }

        return oss.str();
    } catch (...) {
        return "Failed to parse traceback";
    }
}

// ScopedErrorHandler 实现
ScopedErrorHandler::ScopedErrorHandler(ErrorHandler::ErrorCallback callback)
    : m_callback(callback) {
    ErrorHandler::addErrorCallback(m_callback);
    m_registered = true;
}

ScopedErrorHandler::~ScopedErrorHandler() {
    if (m_registered) {
        // Note: Simplified handling here, a more robust implementation
        // might need to remove the specific callback from the list.
    }
}

// ExceptionConverter static member initialization
std::map<std::type_index, std::function<py::object(const std::exception &)>>
    ExceptionConverter::s_to_python_converters;

std::map<std::string, std::function<std::unique_ptr<std::exception>(const py::object &)>>
    ExceptionConverter::s_from_python_converters;

void ExceptionConverter::convertStdException(const std::exception &e) {
    std::type_index type_idx(typeid(e));

    auto it = s_to_python_converters.find(type_idx);
    if (it != s_to_python_converters.end()) {
        py::object py_exception = it->second(e);
        // This should throw a Python exception, but pybind11's mechanism is complex.
        // Simplified handling: just print the error message.
        std::cerr << "C++ Exception converted to Python: " << e.what() << std::endl;
    } else {
        // Default conversion
        std::cerr << "C++ Exception: " << e.what() << std::endl;
    }
}

std::unique_ptr<std::exception> ExceptionConverter::convertPythonException(const py::error_already_set &e) {
    auto error_info = ErrorHandler::extractPythonErrorInfo(e);

    auto it = s_from_python_converters.find(error_info.type);
    if (it != s_from_python_converters.end()) {
        py::object py_exception = py::reinterpret_borrow<py::object>(e.value());
        return it->second(py_exception);
    } else {
        // Default conversion to PythonBridgeException
        return std::make_unique<PythonBridgeException>(ErrorHandler::formatErrorInfo(error_info));
    }
}

} // namespace cpppy_bridge