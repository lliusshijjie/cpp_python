# C++ Python Bridge

一个高性能、易用的C++调用Python代码的桥接库，基于pybind11构建。该库提供了简洁的API接口，让C++开发者能够轻松地在C++应用程序中集成和调用Python代码。

## 🚀 功能特性

- **简单易用**: 提供直观的API接口，轻松实现C++与Python的互操作
- **类型安全**: 自动处理C++和Python之间的类型转换，支持STL容器和自定义类型
- **高性能**: 基于pybind11，提供接近原生的性能表现
- **完善的错误处理**: 全面的异常处理和错误传播机制，支持自定义错误回调
- **灵活配置**: 支持自定义类型转换器和错误处理策略
- **跨平台**: 支持Windows、Linux和macOS
- **现代C++**: 使用C++17标准，支持模板和RAII

## 📋 系统架构

```
┌─────────────────────────────────┐
│         C++ 应用层              │
├─────────────────────────────────┤
│    PythonBridge 包装器层        │  ← 高级API接口
├─────────────────────────────────┤
│  PythonModule | PythonFunction  │  ← 模块和函数封装
├─────────────────────────────────┤
│  TypeConverter | ErrorHandler   │  ← 类型转换和错误处理
├─────────────────────────────────┤
│           pybind11              │  ← 底层绑定库
├─────────────────────────────────┤
│         Python 解释器           │
└─────────────────────────────────┘
```

## 🛠️ 环境要求

- **C++**: C++17 或更高版本
- **Python**: Python 3.6 或更高版本
- **CMake**: CMake 3.12 或更高版本
- **编译器**: 
  - Windows: Visual Studio 2019 或更高版本
  - Linux: GCC 7+ 或 Clang 6+
  - macOS: Xcode 10+ 或 Clang 6+

## 📦 快速开始

### 1. 克隆和编译

```bash
# 克隆项目
git clone <repository-url>
cd CppFromPython

# 创建构建目录
mkdir build
cd build

# 配置和编译
cmake ..
cmake --build . --config Release
```

### 2. 基本使用示例

#### Python脚本 (math_operations.py)
```python
def add(a, b):
    return a + b

def multiply(a, b):
    return a * b

def sum_list(numbers):
    return sum(numbers)
```

#### C++代码
```cpp
#include "python_bridge.h"
#include <iostream>

int main() {
    try {
        // 初始化Python桥接
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"./python_scripts"});
        
        // 加载Python模块
        auto math_module = bridge.loadModule("math_operations");
        
        // 调用Python函数
        double result = math_module->callFunction<double>("add", 10.5, 20.3);
        std::cout << "Result: " << result << std::endl;  // 输出: 30.8
        
        // 处理容器类型
        std::vector<double> numbers = {1.0, 2.0, 3.0, 4.0, 5.0};
        double sum = math_module->callFunction<double>("sum_list", numbers);
        std::cout << "Sum: " << sum << std::endl;  // 输出: 15.0
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

## 🔧 核心组件

### PythonBridge
主要的桥接类，负责管理Python解释器和模块加载：
```cpp
cpppy_bridge::PythonBridge bridge;
bridge.initialize({"./python_scripts"});
auto module = bridge.loadModule("my_module");
```

### PythonModule
Python模块的封装类，提供函数调用接口：
```cpp
auto result = module->callFunction<int>("my_function", arg1, arg2);
bool has_func = module->hasFunction("function_name");
```

### PythonFunction
Python函数的封装类，提供更便捷的调用方式：
```cpp
cpppy_bridge::PythonFunction func("module_name", "function_name");
auto result = func.call<double>(1.0, 2.0);
```

### TypeConverter
类型转换工具，支持C++和Python之间的自动类型转换：
```cpp
// 支持的类型包括：
// - 基本类型: int, double, float, bool, string
// - STL容器: vector, map, set, tuple
// - 复杂类型: optional, variant
// - NumPy数组
```

### ErrorHandler
错误处理系统，提供异常捕获和错误回调：
```cpp
cpppy_bridge::ErrorHandler::setVerboseErrors(true);
cpppy_bridge::ErrorHandler::addErrorCallback([](const auto& error) {
    std::cout << "Python Error: " << error.message << std::endl;
});
```

## 📚 高级功能

### 1. 自定义类型转换
```cpp
// 注册自定义类型转换器
REGISTER_CUSTOM_TYPE_CONVERTER(
    MyCustomType,
    [](const MyCustomType& obj) { return py::cast(obj); },
    [](const py::object& obj) { return obj.cast<MyCustomType>(); }
);
```

### 2. 错误处理和安全执行
```cpp
// 安全执行Python代码
auto result = cpppy_bridge::ErrorHandler::safeExecuteOptional([&]() {
    return module->callFunction<double>("risky_function");
});

if (result.has_value()) {
    std::cout << "Result: " << result.value() << std::endl;
} else {
    std::cout << "Function execution failed" << std::endl;
}
```

### 3. NumPy数组支持
```cpp
// C++ vector 转 NumPy数组
std::vector<double> data = {1.0, 2.0, 3.0, 4.0};
auto numpy_array = cpppy_bridge::NumpyConverter::vectorToNumpy(data);

// NumPy数组转 C++ vector
auto cpp_vector = cpppy_bridge::NumpyConverter::numpyToVector<double>(numpy_array);
```

## 🧪 测试和示例

### 运行示例
```bash
# 在build目录中
./example          # 运行示例程序
./test_bridge       # 运行测试套件
```

### 示例功能
- 基本函数调用
- 容器操作（vector, map等）
- 复杂数据结构处理
- 错误处理演示
- 函数包装器使用
- 全局状态管理

## 📖 API文档

详细的API文档请参考：[API Reference](docs/API_Reference.md)

## 🤝 贡献指南

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🔗 相关链接

- [pybind11 官方文档](https://pybind11.readthedocs.io/)
- [Python C API 文档](https://docs.python.org/3/c-api/)
- [CMake 官方文档](https://cmake.org/documentation/)

## ❓ 常见问题

### Q: 如何处理Python模块导入错误？
A: 确保Python模块路径正确添加到搜索路径中，使用 `bridge.initialize({"path/to/modules"})` 或 `PythonInterpreter::getInstance().addModulePath()`。

### Q: 支持哪些数据类型？
A: 支持所有基本类型、STL容器、NumPy数组以及自定义类型（通过注册转换器）。

### Q: 如何调试Python代码？
A: 启用详细错误模式 `ErrorHandler::setVerboseErrors(true)` 和错误日志 `ErrorHandler::setErrorLogging(true)`。

### Q: 性能如何？
A: 基于pybind11，提供接近原生的性能，类型转换开销最小。

---

**注意**: 本项目使用 `cpppy_bridge` 命名空间来避免与标准库的命名冲突。