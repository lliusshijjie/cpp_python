# C++ Python Bridge

A high-performance, easy-to-use C++ library for seamless interoperability between C++ and Python, built on the power of pybind11. This library provides a clean, modern C++ API for calling Python from C++.

## 🚀 Core Features

- **C++ calling Python**: Embed the Python interpreter in your C++ application. Load modules, call functions, and exchange complex data types with ease.
- **Powerful Type Conversion**: Automatic and extensible type conversion for a wide range of types, including:
    - Primitives (`int`, `double`, `std::string`, etc.)
    - STL Containers (`std::vector`, `std::map`, `std::tuple`, `std::optional`)
    - NumPy arrays
    - Custom user-defined types via a simple registration system.
- **Robust Error Handling**: A comprehensive exception handling system that translates Python exceptions into C++ exceptions, providing detailed error information including stack traces.
- **Modern C++ Design**: Utilizes C++17 features for a clean, safe, and efficient API.

## 📋 System Architecture

The library is composed of the C++ to Python Bridge, for embedding Python in C++.

```
┌───────────────────────────────┐
│          C++ Application      │
├───────────────────────────────┤
│        PythonBridge API       │  <-- High-level Facade
├───────────────────────────────┤
│  PythonModule | PythonFunction│  <-- Module/Function Wrappers
└───────────────────────────────┘
```

Under the hood, the bridge is powered by a shared core that handles type conversion, error handling, and the low-level `pybind11` interface.

## 🛠️ Requirements

- C++17 or higher
- Python 3.6 or higher
- CMake 3.12 or higher
- A modern C++ compiler (Visual Studio 2019+, GCC 7+, Clang 6+)
- `pybind11` is downloaded automatically by CMake.

## 📦 Quick Start

### 1. Build the Project

```bash
# Clone the project
git clone <repository-url>
cd CppFromPython

# Configure and build using CMake
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 2. Example: C++ calling Python

This example calls a Python function from C++.

**Python Script (`examples/python_scripts/math_operations.py`)**
```python
def add(a, b):
    return a + b
```

**C++ Code (`examples/main.cpp`)**
```cpp
#include "python_bridge.h"
#include <iostream>
#include <vector>

int main() {
    try {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"./examples/python_scripts"});
        
        auto math_module = bridge.loadModule("math_operations");
        
        double result = math_module->callFunction<double>("add", 10.5, 20.3);
        std::cout << "Python says: 10.5 + 20.3 = " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }
    return 0;
}
```

**Run the example:**
```bash
# From the build directory
./cpp_to_python_example
```

## 📖 Documentation

For more detailed information, please refer to:
- **[API Reference](docs/API_Reference.md)**: A detailed reference for all classes and functions.
- **[Bridge Guide](docs/Bidirectional_Bridge_Guide.md)**: A comprehensive guide with tutorials and advanced usage examples.

## 🤝 Contributing

Contributions are welcome! Please follow the standard fork-and-pull-request workflow.

## 📄 License

This project is licensed under the MIT License.