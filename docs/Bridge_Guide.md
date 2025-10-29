# C++ Python Bridge 开发者指南

## 概述

本指南提供了 C++ Python Bridge 库的实用教程，涵盖从 C++ 调用 Python 的工作流程，包含详细示例和最佳实践。

---

## 目录

1. [快速开始](#快速开始)
2. [基础用法](#基础用法)
3. [类型转换](#类型转换)
4. [错误处理](#错误处理)
5. [性能优化](#性能优化)
6. [最佳实践](#最佳实践)
7. [常见问题](#常见问题)

---

## 快速开始

### 最简示例

```cpp
#include "python_bridge.h"
#include <iostream>

int main() {
    cpppy_bridge::PythonBridge bridge;
    bridge.initialize({"./python_scripts"});
    
    auto module = bridge.loadModule("my_module");
    double result = module->callFunction<double>("add", 10.5, 20.3);
    
    std::cout << "Result: " << result << std::endl;
    return 0;
}
```

**对应的 Python 脚本** (`python_scripts/my_module.py`):
```python
def add(a, b):
    return a + b
```

---

## 基础用法

### 1. 初始化 Python 桥接

第一步始终是初始化 `PythonBridge`，这会设置嵌入式 Python 解释器。

```cpp
#include "python_bridge.h"

int main() {
    cpppy_bridge::PythonBridge bridge;
    
    // 添加多个 Python 脚本搜索路径
    bridge.initialize({
        "./python_scripts",
        "./external_modules",
        "/usr/local/lib/python_modules"
    });
    
    // 现在可以加载模块了...
}
```

### 2. 加载 Python 模块

初始化后，可以加载任何在搜索路径中的 Python 模块。

```cpp
try {
    auto math_module = bridge.loadModule("math_operations");
    
    if (math_module) {
        std::cout << "模块加载成功！" << std::endl;
    }
} catch (const cpppy_bridge::PythonModuleException& e) {
    std::cerr << "模块加载失败: " << e.what() << std::endl;
}
```

**提示**: 模块会被自动缓存，重复加载同一模块不会有额外开销。

### 3. 调用 Python 函数

#### 3.1 基础类型

```cpp
// Python 函数定义
// def calculate(x, y, operation):
//     if operation == "add":
//         return x + y
//     elif operation == "multiply":
//         return x * y

auto module = bridge.loadModule("calculator");

// 调用并接收返回值
double sum = module->callFunction<double>("calculate", 10.5, 20.3, "add");
double product = module->callFunction<double>("calculate", 5.0, 4.0, "multiply");

std::cout << "Sum: " << sum << std::endl;       // 30.8
std::cout << "Product: " << product << std::endl; // 20.0
```

#### 3.2 STL 容器

```cpp
// Python 函数
// def process_list(numbers):
//     return {
//         "sum": sum(numbers),
//         "average": sum(numbers) / len(numbers),
//         "max": max(numbers),
//         "min": min(numbers)
//     }

std::vector<double> numbers = {1.5, 2.7, 3.9, 4.1, 5.3};
auto result = module->callFunction<std::map<std::string, double>>(
    "process_list", 
    numbers
);

std::cout << "Sum: " << result["sum"] << std::endl;
std::cout << "Average: " << result["average"] << std::endl;
```

#### 3.3 复杂嵌套结构

```cpp
// Python 函数
// def analyze_datasets(data):
//     results = {}
//     for name, values in data.items():
//         results[name] = {
//             "count": len(values),
//             "sum": sum(values),
//             "mean": sum(values) / len(values)
//         }
//     return results

std::map<std::string, std::vector<double>> datasets = {
    {"experiment_1", {1.0, 2.0, 3.0, 4.0, 5.0}},
    {"experiment_2", {10.5, 20.3, 30.7, 40.1}},
    {"experiment_3", {100.0, 200.0, 300.0}}
};

using ResultType = std::map<std::string, std::map<std::string, double>>;
auto analysis = module->callFunction<ResultType>("analyze_datasets", datasets);

for (const auto& [name, stats] : analysis) {
    std::cout << name << ":" << std::endl;
    std::cout << "  Count: " << stats.at("count") << std::endl;
    std::cout << "  Mean: " << stats.at("mean") << std::endl;
}
```

### 4. 使用函数包装器（高性能）

如果需要多次调用同一个函数，使用 `PythonFunction` 包装器可以获得更好的性能。

```cpp
auto module = bridge.loadModule("math_operations");

// 创建函数包装器
cpppy_bridge::PythonFunction fibonacci(module, "fibonacci");

if (fibonacci.isValid()) {
    // 多次调用，无需重复查找函数
    for (int i = 1; i <= 10; i++) {
        int result = fibonacci.call<int>(i);
        std::cout << "F(" << i << ") = " << result << std::endl;
    }
}
```

---

## 类型转换

### 支持的类型

#### 基础类型
- `int`, `long`, `float`, `double`
- `bool`
- `std::string`

#### STL 容器
- `std::vector<T>`
- `std::map<K, V>`
- `std::tuple<T...>`
- `std::optional<T>`
- `std::set<T>`

#### 特殊类型
- `py::object` - 直接使用 pybind11 对象
- NumPy 数组（需要包含相应头文件）

### 自定义类型转换

如果需要支持自定义类型，可以注册转换器：

```cpp
#include "type_converter.h"

struct Point {
    double x, y;
};

// 注册转换器
REGISTER_CUSTOM_TYPE_CONVERTER(
    Point,
    // C++ -> Python
    [](const Point& p) {
        py::dict d;
        d["x"] = p.x;
        d["y"] = p.y;
        return d;
    },
    // Python -> C++
    [](const py::object& obj) {
        Point p;
        p.x = obj["x"].cast<double>();
        p.y = obj["y"].cast<double>();
        return p;
    }
);
```

---

## 错误处理

### 基础异常处理

```cpp
try {
    auto result = module->callFunction<double>("risky_operation", 42);
} catch (const cpppy_bridge::PythonFunctionException& e) {
    std::cerr << "函数调用错误: " << e.what() << std::endl;
} catch (const cpppy_bridge::PythonModuleException& e) {
    std::cerr << "模块错误: " << e.what() << std::endl;
} catch (const cpppy_bridge::PythonBridgeException& e) {
    std::cerr << "桥接错误: " << e.what() << std::endl;
}
```

### 自定义错误回调

```cpp
#include "error_handler.h"

// 设置全局错误回调
cpppy_bridge::ErrorHandler::addErrorCallback(
    [](const cpppy_bridge::PythonErrorInfo& info) {
        std::cerr << "[Python Error]" << std::endl;
        std::cerr << "  Type: " << info.type << std::endl;
        std::cerr << "  Message: " << info.message << std::endl;
        std::cerr << "  File: " << info.file << ":" << info.line << std::endl;
        std::cerr << "  Traceback:\n" << info.traceback << std::endl;
        
        // 可以记录到日志文件、发送告警等
    }
);
```

### 安全执行模式

```cpp
#include "error_handler.h"

// 返回 std::optional，失败时返回 std::nullopt
auto result = cpppy_bridge::ErrorHandler::safeExecuteOptional([&]() {
    return module->callFunction<double>("might_fail", 42);
});

if (result) {
    std::cout << "成功: " << *result << std::endl;
} else {
    std::cout << "执行失败，已自动处理" << std::endl;
}
```

---

## 性能优化

### 1. 使用函数包装器

```cpp
// ❌ 慢：每次调用都要查找函数
for (int i = 0; i < 1000; i++) {
    module->callFunction<int>("compute", i);
}

// ✅ 快：函数查找只进行一次
cpppy_bridge::PythonFunction compute(module, "compute");
for (int i = 0; i < 1000; i++) {
    compute.call<int>(i);
}
```

**性能提升**: 约 2-3 倍

### 2. 批量处理

```cpp
// ❌ 低效：逐个处理
std::vector<int> results;
for (int value : large_dataset) {
    int result = module->callFunction<int>("process", value);
    results.push_back(result);
}

// ✅ 高效：批量处理
std::vector<int> results = module->callFunction<std::vector<int>>(
    "process_batch", 
    large_dataset
);
```

### 3. 模块复用

```cpp
// ✅ 好：模块自动缓存
auto module1 = bridge.loadModule("calculations");
auto module2 = bridge.loadModule("calculations"); // 返回缓存的模块

// 获取已加载的模块
auto module3 = bridge.getModule("calculations");
```

### 4. 避免频繁的类型转换

```cpp
// ❌ 低效
for (auto& item : large_data) {
    auto py_item = TypeConverter::toPython(item);
    // 处理 py_item...
}

// ✅ 高效：一次转换整个容器
auto py_data = TypeConverter::toPython(large_data);
```

---

## 最佳实践

### 1. 资源管理

```cpp
class MyApplication {
private:
    cpppy_bridge::PythonBridge bridge_;
    std::shared_ptr<cpppy_bridge::PythonModule> module_;
    
public:
    MyApplication() {
        bridge_.initialize({"./scripts"});
        module_ = bridge_.loadModule("app_logic");
    }
    
    // PythonBridge 析构时会自动清理 Python 解释器
};
```

### 2. 线程安全

```cpp
// Python GIL (Global Interpreter Lock) 注意事项
// 本库不是线程安全的，如需多线程使用，请加锁

std::mutex python_mutex;

void thread_function() {
    std::lock_guard<std::mutex> lock(python_mutex);
    auto result = module->callFunction<double>("compute", 42);
}
```

### 3. 错误处理策略

```cpp
// 关键路径：立即失败
double critical_calc = module->callFunction<double>("critical_operation", data);

// 非关键路径：优雅降级
auto optional_result = ErrorHandler::safeExecuteOptional([&]() {
    return module->callFunction<double>("optional_feature", data);
});

if (optional_result) {
    use_enhanced_feature(*optional_result);
} else {
    use_fallback_feature();
}
```

### 4. 调试技巧

```cpp
// 启用详细错误信息
cpppy_bridge::ErrorHandler::setVerboseErrors(true);
cpppy_bridge::ErrorHandler::setErrorLogging(true);

// 检查函数是否存在
if (module->hasFunction("my_function")) {
    // 安全调用
    auto result = module->callFunction<int>("my_function", args...);
}
```

---

## 常见问题

### Q1: 找不到 Python 模块

**问题**: `ModuleNotFoundError: No module named 'xxx'`

**解决**:
```cpp
// 确保添加了正确的搜索路径
bridge.initialize({
    "./python_scripts",      // 相对路径
    "/absolute/path/to/scripts"  // 绝对路径
});

// 或者使用 Python 的 sys.path
auto& interpreter = cpppy_bridge::PythonInterpreter::getInstance();
interpreter.addModulePath("your/custom/path");
```

### Q2: 类型转换失败

**问题**: `TypeConversionException`

**解决**:
```cpp
// 使用 py::object 作为中间类型
auto py_result = module->callFunction<py::object>("function", args);

// 手动转换
if (py::isinstance<py::int_>(py_result)) {
    int value = py_result.cast<int>();
}
```

### Q3: 性能不理想

**解决方案**:
1. 使用函数包装器减少查找开销
2. 批量处理数据而非逐个处理
3. 避免频繁的 C++ ↔ Python 边界跨越
4. 考虑在 Python 端使用 NumPy 等优化库

### Q4: 内存泄漏

**解决**:
- 使用智能指针（`std::shared_ptr`）
- 确保 PythonBridge 对象在程序结束时正确析构
- 不要手动管理 `py::object` 的生命周期

---

## 性能基准

在测试环境中（Windows 11, Python 3.12, MSVC 2022）：

| 操作 | 平均耗时 |
|------|----------|
| 简单函数调用 | 0.072 μs |
| 带参数的函数调用 | 0.15 μs |
| 容器类型转换 | 0.5 μs |
| 复杂嵌套结构 | 2-5 μs |

---

## 下一步

- 查看 [API Reference](./API_Reference.md) 了解完整的 API 文档
- 研究 `examples/main.cpp` 中的完整示例
- 阅读 `examples/python_scripts/math_operations.py` 了解 Python 端的最佳实践

---

**提示**: 这是一个活跃开发的项目，欢迎贡献代码和反馈问题！