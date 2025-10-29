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

### 前置要求

在开始之前，请确保已安装：

1. **Python 3.6+** 和 pip
2. **CMake 3.12+**
3. **Visual Studio 2019+** (Windows) 或 **GCC 7+/Clang 6+** (Linux/macOS)
4. **Windows SDK** (仅 Windows)

安装 Python 依赖：
```bash
pip install pybind11 numpy
```

### Windows 平台构建

#### 方法1: 使用提供的脚本（推荐）

```batch
# 1. 配置 CMake
build_script.bat

# 2. 编译项目
compile.bat

# 3. 运行示例
cd build
cpp_to_python_example.exe
```

#### 方法2: 手动构建

```batch
# 1. 创建 build 目录
mkdir build
cd build

# 2. 配置 CMake（需要指定 pybind11 路径）
cmake -G "NMake Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH="%PYTHON_PATH%\Lib\site-packages\pybind11\share\cmake\pybind11" ^
      ..

# 3. 编译
nmake

# 4. 运行
cpp_to_python_example.exe
```

### Linux/macOS 平台构建

```bash
# 1. 创建并进入 build 目录
mkdir build && cd build

# 2. 配置和编译
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# 3. 运行
./cpp_to_python_example
```

### 示例：C++ 调用 Python

这个例子展示了如何从 C++ 调用 Python 函数。

**Python 脚本 (`examples/python_scripts/math_operations.py`)**
```python
def add(a, b):
    """简单的加法函数"""
    return a + b

def process_data(data_dict):
    """处理复杂数据结构"""
    result = {}
    for key, values in data_dict.items():
        result[key] = {
            "sum": sum(values),
            "average": sum(values) / len(values),
            "count": len(values)
        }
    return result
```

**C++ 代码 (`examples/main.cpp`)**
```cpp
#include "python_bridge.h"
#include <iostream>
#include <map>
#include <vector>

int main() {
    try {
        // 初始化桥接
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});
        
        // 加载 Python 模块
        auto math_module = bridge.loadModule("math_operations");
        
        // 调用简单函数
        double result = math_module->callFunction<double>("add", 10.5, 20.3);
        std::cout << "10.5 + 20.3 = " << result << std::endl;
        
        // 调用复杂函数（传递嵌套数据结构）
        std::map<std::string, std::vector<double>> data = {
            {"dataset1", {1.0, 2.0, 3.0, 4.0, 5.0}},
            {"dataset2", {10.5, 20.3, 30.7}}
        };
        auto processed = math_module->callFunction<py::object>("process_data", data);
        
        // 处理返回的数据...
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

### 运行示例程序

编译成功后，运行示例程序将展示以下功能：

- ✅ 基础函数调用（加法、乘法、阶乘、斐波那契）
- ✅ 容器操作（列表求和、平均值、矩阵乘法）
- ✅ 复杂数据结构处理
- ✅ 异常处理（ValueError, TypeError, RuntimeError等）
- ✅ 函数包装器（高性能重复调用）
- ✅ 全局状态管理

**性能**: 每次函数调用仅需约 **0.072 微秒** ⚡

## 📖 Documentation

For more detailed information, please refer to:
- **[API Reference](docs/API_Reference.md)**: A detailed reference for all classes and functions.
- **[Bridge Guide](docs/Bidirectional_Bridge_Guide.md)**: A comprehensive guide with tutorials and advanced usage examples.

## 🤝 Contributing

Contributions are welcome! Please follow the standard fork-and-pull-request workflow.

## 📄 License

This project is licensed under the MIT License.