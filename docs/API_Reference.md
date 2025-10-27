# API Reference

This document provides a detailed API reference for the C++ Python Bridge library.

## Part 1: C++ calling Python (`<python_bridge.h>`)

This part of the API allows C++ applications to embed and interact with the Python interpreter.

### `cpppy_bridge::PythonBridge`
The main high-level interface for managing the Python environment from C++.

- **`PythonBridge()`**: Constructor.
- **`~PythonBridge()`**: Destructor. Finalizes the Python interpreter.
- **`bool initialize(const std::vector<std::string>& module_paths = {})`**: Initializes the Python interpreter and adds specified paths to the Python module search path.
- **`std::shared_ptr<PythonModule> loadModule(const std::string& module_name)`**: Loads a Python module by name.
- **`std::shared_ptr<PythonModule> getModule(const std::string& module_name)`**: Retrieves a previously loaded module.
- **`std::shared_ptr<PythonFunction> createFunction(const std::string& module_name, const std::string& func_name)`**: Creates a wrapper for a specific Python function.
- **`py::object executeFile(const std::string& file_path)`**: Executes a Python script from a file.
- **`py::object executeCode(const std::string& code)`**: Executes a string of Python code.

### `cpppy_bridge::PythonModule`
A wrapper around a Python module object (`py::module`).

- **`explicit PythonModule(const std::string& module_name)`**: Constructor that loads a Python module.
- **`bool isLoaded() const`**: Returns `true` if the module was loaded successfully.
- **`bool hasFunction(const std::string& func_name) const`**: Checks if a function exists in the module.
- **`template<typename R, typename... Args> R callFunction(const std::string& func_name, Args&&... args)`**: Calls a function within the module with the given arguments and returns a value of type `R`.
- **`py::object getAttribute(const std::string& attr_name)`**: Gets a module attribute (e.g., a variable or class).
- **`void setAttribute(const std::string& attr_name, const py::object& value)`**: Sets a module attribute.

### `cpppy_bridge::PythonFunction`
A convenient wrapper for a single Python function.

- **`PythonFunction(const std::string& module_name, const std::string& func_name)`**: Constructor.
- **`bool isValid() const`**: Returns `true` if the function was found and is callable.
- **`template<typename R, typename... Args> R operator()(Args&&... args)`**: Calls the Python function.

---

## Part 2: Shared Core

### Type Conversion (`<type_converter.h>`)
The library handles most type conversions automatically. For custom types, you can register a converter.

- **`REGISTER_CUSTOM_TYPE_CONVERTER(CppType, ToPythonFunc, FromPythonFunc)`**: A macro to register conversion functions for a custom C++ type `CppType`.

### Error Handling (`<error_handler.h>`)
- **`cpppy_bridge::PythonBridgeException`**: Base exception class for the library.
- **`ErrorHandler::setGlobalErrorCallback(ErrorCallback callback)`**: Sets a global callback function to be invoked whenever a Python exception occurs. The callback receives a `PythonErrorInfo` struct.
- **`struct PythonErrorInfo`**: Contains detailed information about a Python error (`type`, `message`, `traceback`, `file`, `line`, `function`).