# API 参考手册

本文档提供了 C++ Python Bridge 库的详细 API 参考。

---

## 目录

1. [核心类](#核心类)
   - [PythonBridge](#pythonbridge)
   - [PythonModule](#pythonmodule)
   - [PythonFunction](#pythonfunction)
   - [PythonInterpreter](#pythoninterpreter)
2. [类型转换](#类型转换)
3. [错误处理](#错误处理)
4. [工具宏](#工具宏)

---

## 核心类

### PythonBridge

**头文件**: `<python_bridge.h>`

主要的高层接口，用于从 C++ 管理 Python 环境。

#### 构造与析构

```cpp
PythonBridge()
```
- **说明**: 构造函数，创建桥接对象
- **示例**:
  ```cpp
  cpppy_bridge::PythonBridge bridge;
  ```

```cpp
~PythonBridge()
```
- **说明**: 析构函数，自动清理 Python 解释器资源

#### 初始化

```cpp
bool initialize(const std::vector<std::string>& module_paths = {})
```
- **参数**:
  - `module_paths`: Python 模块搜索路径列表（可选）
- **返回值**: 成功返回 `true`，失败返回 `false`
- **说明**: 初始化 Python 解释器并添加指定路径到模块搜索路径
- **示例**:
  ```cpp
  bridge.initialize({
      "./python_scripts",
      "/usr/local/lib/python_modules"
  });
  ```

#### 模块管理

```cpp
std::shared_ptr<PythonModule> loadModule(const std::string& module_name)
```
- **参数**:
  - `module_name`: 要加载的 Python 模块名称
- **返回值**: 指向 `PythonModule` 对象的智能指针，失败返回 `nullptr`
- **说明**: 加载指定的 Python 模块（自动缓存）
- **异常**: 可能抛出 `PythonModuleException`
- **示例**:
  ```cpp
  auto module = bridge.loadModule("math_operations");
  if (module) {
      // 使用模块...
  }
  ```

```cpp
std::shared_ptr<PythonModule> getModule(const std::string& module_name)
```
- **参数**:
  - `module_name`: 模块名称
- **返回值**: 指向已加载模块的智能指针，未找到返回 `nullptr`
- **说明**: 获取之前已加载的模块（不会尝试加载新模块）
- **示例**:
  ```cpp
  auto cached_module = bridge.getModule("math_operations");
  ```

#### 函数管理

```cpp
std::shared_ptr<PythonFunction> createFunction(
    const std::string& module_name, 
    const std::string& func_name
)
```
- **参数**:
  - `module_name`: 模块名称
  - `func_name`: 函数名称
- **返回值**: 指向 `PythonFunction` 包装器的智能指针
- **说明**: 创建特定 Python 函数的包装器（用于高性能重复调用）
- **示例**:
  ```cpp
  auto add_func = bridge.createFunction("math_operations", "add");
  ```

#### 代码执行

```cpp
py::object executeFile(const std::string& file_path)
```
- **参数**:
  - `file_path`: Python 脚本文件路径
- **返回值**: 执行结果（`py::object`）
- **说明**: 执行 Python 脚本文件
- **异常**: 文件不存在或执行失败时抛出异常
- **示例**:
  ```cpp
  bridge.executeFile("./scripts/init_script.py");
  ```

```cpp
py::object executeCode(const std::string& code)
```
- **参数**:
  - `code`: Python 代码字符串
- **返回值**: 执行结果（`py::object`）
- **说明**: 执行 Python 代码字符串
- **示例**:
  ```cpp
  auto result = bridge.executeCode("2 + 3");
  int value = result.cast<int>();  // 5
  ```

---

### PythonModule

**头文件**: `<python_bridge.h>`

Python 模块对象的包装器。

#### 构造函数

```cpp
explicit PythonModule(const std::string& module_name)
```
- **参数**:
  - `module_name`: 要加载的模块名称
- **说明**: 构造函数，加载指定的 Python 模块

#### 状态查询

```cpp
bool isLoaded() const
```
- **返回值**: 模块成功加载返回 `true`
- **说明**: 检查模块是否已成功加载

```cpp
bool hasFunction(const std::string& func_name) const
```
- **参数**:
  - `func_name`: 函数名称
- **返回值**: 函数存在返回 `true`
- **说明**: 检查模块中是否存在指定函数

#### 函数调用

```cpp
template<typename ReturnType, typename... Args>
ReturnType callFunction(const std::string& func_name, Args&&... args)
```
- **模板参数**:
  - `ReturnType`: 返回值类型
  - `Args`: 参数类型（可变参数）
- **参数**:
  - `func_name`: 要调用的函数名称
  - `args`: 函数参数
- **返回值**: 指定类型的返回值
- **说明**: 调用模块中的函数，自动进行类型转换
- **异常**: 调用失败时抛出 `PythonFunctionException`
- **示例**:
  ```cpp
  // 简单调用
  double result = module->callFunction<double>("add", 10.5, 20.3);
  
  // 容器参数
  std::vector<int> data = {1, 2, 3, 4, 5};
  double sum = module->callFunction<double>("sum_list", data);
  
  // 复杂返回类型
  auto stats = module->callFunction<std::map<std::string, double>>(
      "calculate_stats", data
  );
  ```

```cpp
py::object callFunction(const std::string& func_name, 
                        const std::vector<py::object>& args = {})
```
- **参数**:
  - `func_name`: 函数名称
  - `args`: Python 对象参数列表
- **返回值**: `py::object`
- **说明**: 使用 pybind11 对象直接调用函数

#### 属性访问

```cpp
py::object getAttribute(const std::string& attr_name)
```
- **参数**:
  - `attr_name`: 属性名称
- **返回值**: 属性值（`py::object`）
- **说明**: 获取模块属性（变量、类、常量等）
- **示例**:
  ```cpp
  auto const_pi = module->getAttribute("PI");
  double pi = const_pi.cast<double>();
  ```

```cpp
void setAttribute(const std::string& attr_name, const py::object& value)
```
- **参数**:
  - `attr_name`: 属性名称
  - `value`: 要设置的值
- **说明**: 设置模块属性
- **示例**:
  ```cpp
  module->setAttribute("global_config", py::dict("debug"_a=true));
  ```

---

### PythonFunction

**头文件**: `<python_bridge.h>`

单个 Python 函数的便捷包装器，适合高性能重复调用。

#### 构造函数

```cpp
PythonFunction(const std::string& module_name, 
               const std::string& func_name)
```
- **参数**:
  - `module_name`: 模块名称
  - `func_name`: 函数名称

```cpp
PythonFunction(std::shared_ptr<PythonModule> module, 
               const std::string& func_name)
```
- **参数**:
  - `module`: 已加载的模块指针
  - `func_name`: 函数名称

#### 函数调用

```cpp
bool isValid() const
```
- **返回值**: 函数有效且可调用返回 `true`
- **说明**: 检查函数包装器是否有效

```cpp
template<typename ReturnType, typename... Args>
ReturnType operator()(Args&&... args)
```
- **说明**: 使用函数调用运算符调用 Python 函数
- **示例**:
  ```cpp
  PythonFunction add_func(module, "add");
  double result = add_func(10.5, 20.3);
  ```

```cpp
template<typename ReturnType, typename... Args>
ReturnType call(Args&&... args)
```
- **说明**: 显式调用函数
- **示例**:
  ```cpp
  double result = add_func.call<double>(10.5, 20.3);
  ```

```cpp
py::object callPy(const std::vector<py::object>& args = {})
```
- **说明**: 使用 pybind11 对象调用函数

---

### PythonInterpreter

**头文件**: `<python_bridge.h>`

Python 解释器管理器（单例模式）。

#### 获取实例

```cpp
static PythonInterpreter& getInstance()
```
- **返回值**: 单例实例的引用
- **说明**: 获取全局 Python 解释器实例

#### 初始化控制

```cpp
void initialize()
```
- **说明**: 初始化 Python 解释器（自动调用，通常无需手动调用）

```cpp
void finalize()
```
- **说明**: 终止 Python 解释器

```cpp
bool isInitialized() const
```
- **返回值**: 解释器已初始化返回 `true`

#### 路径管理

```cpp
void addModulePath(const std::string& path)
```
- **参数**:
  - `path`: 要添加的模块搜索路径
- **说明**: 添加路径到 Python 的 `sys.path`
- **示例**:
  ```cpp
  auto& interpreter = PythonInterpreter::getInstance();
  interpreter.addModulePath("/custom/python/lib");
  ```

#### 代码执行

```cpp
py::object execute(const std::string& code)
```
- **参数**:
  - `code`: Python 代码字符串
- **返回值**: 执行结果
- **说明**: 执行 Python 代码

---

## 类型转换

### TypeConverter

**头文件**: `<type_converter.h>`

提供 C++ 和 Python 之间的类型转换功能。

#### 支持的类型

**基础类型**:
- `int`, `long`, `long long`
- `float`, `double`
- `bool`
- `std::string`

**STL 容器**:
- `std::vector<T>`
- `std::map<K, V>`, `std::unordered_map<K, V>`
- `std::tuple<T...>`
- `std::optional<T>`
- `std::set<T>`, `std::unordered_set<T>`

#### 转换函数

```cpp
template<typename T>
static py::object toPython(const T& value)
```
- **说明**: 将 C++ 类型转换为 Python 对象

```cpp
template<typename T>
static T fromPython(const py::object& obj)
```
- **说明**: 将 Python 对象转换为 C++ 类型

```cpp
template<typename T>
static bool canConvert(const py::object& obj)
```
- **说明**: 检查 Python 对象是否可转换为指定 C++ 类型

```cpp
template<typename T>
static std::optional<T> safeCast(const py::object& obj)
```
- **说明**: 安全转换，失败返回 `std::nullopt`

---

## 错误处理

### 异常类层次

```
PythonBridgeException (基类)
├── PythonInterpreterException   // 解释器错误
├── PythonModuleException        // 模块加载错误
├── PythonFunctionException      // 函数调用错误
└── TypeConversionException      // 类型转换错误
```

### ErrorHandler

**头文件**: `<error_handler.h>`

#### 错误回调

```cpp
static void setGlobalErrorCallback(ErrorCallback callback)
```
- **参数**:
  - `callback`: 错误回调函数
- **说明**: 设置全局错误回调

```cpp
static void addErrorCallback(ErrorCallback callback)
```
- **说明**: 添加错误回调（支持多个回调）

```cpp
static void clearErrorCallbacks()
```
- **说明**: 清除所有错误回调

#### 异常处理

```cpp
static PythonErrorInfo handlePythonException(const py::error_already_set& e)
```
- **返回值**: 错误信息结构体
- **说明**: 处理 Python 异常，提取详细信息

```cpp
static void convertPythonException(const py::error_already_set& e)
```
- **说明**: 将 Python 异常转换为 C++ 异常并抛出

#### 安全执行

```cpp
template<typename Func>
static auto safeExecute(Func&& func) -> decltype(func())
```
- **说明**: 安全执行函数，自动处理异常

```cpp
template<typename Func>
static auto safeExecuteOptional(Func&& func) 
    -> std::optional<decltype(func())>
```
- **说明**: 安全执行，失败返回 `std::nullopt`

#### 配置

```cpp
static void setVerboseErrors(bool verbose)
```
- **说明**: 启用/禁用详细错误信息

```cpp
static void setErrorLogging(bool enable)
```
- **说明**: 启用/禁用错误日志

### PythonErrorInfo

```cpp
struct PythonErrorInfo {
    std::string type;        // 异常类型
    std::string message;     // 错误消息
    std::string traceback;   // 堆栈跟踪
    std::string file;        // 文件名
    int line = -1;           // 行号
    std::string function;    // 函数名
};
```

---

## 工具宏

### 类型转换

```cpp
REGISTER_CUSTOM_TYPE_CONVERTER(CppType, ToPythonFunc, FromPythonFunc)
```
- **说明**: 注册自定义类型转换器
- **示例**:
  ```cpp
  struct Point { double x, y; };
  
  REGISTER_CUSTOM_TYPE_CONVERTER(
      Point,
      [](const Point& p) { /* C++ -> Python */ },
      [](const py::object& obj) { /* Python -> C++ */ }
  );
  ```

### 错误处理

```cpp
SAFE_PYTHON_CALL(call)
```
- **说明**: 安全执行 Python 调用的宏

```cpp
SAFE_PYTHON_CALL_OPTIONAL(call)
```
- **说明**: 安全执行，返回 `std::optional`

```cpp
HANDLE_PYTHON_EXCEPTION(e)
```
- **说明**: 处理 Python 异常的宏

---

## 使用示例

### 完整示例

```cpp
#include "python_bridge.h"
#include "error_handler.h"
#include <iostream>

int main() {
    try {
        // 初始化
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"./python_scripts"});
        
        // 启用详细错误
        cpppy_bridge::ErrorHandler::setVerboseErrors(true);
        
        // 加载模块
        auto module = bridge.loadModule("math_operations");
        
        // 调用函数
        double result = module->callFunction<double>("add", 10.5, 20.3);
        std::cout << "Result: " << result << std::endl;
        
        // 使用函数包装器
        cpppy_bridge::PythonFunction fib(module, "fibonacci");
        for (int i = 1; i <= 10; i++) {
            std::cout << "F(" << i << ") = " 
                      << fib.call<int>(i) << std::endl;
        }
        
    } catch (const cpppy_bridge::PythonBridgeException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

---

## 性能说明

- **函数查找开销**: 每次调用约 0.05 μs
- **函数调用开销**: 约 0.072 μs
- **类型转换开销**: 基础类型约 0.01 μs，容器约 0.5 μs

**优化建议**:
1. 对重复调用使用 `PythonFunction` 包装器
2. 批量处理数据而非逐个调用
3. 避免频繁的类型转换

---

**版本**: 1.0.0  
**最后更新**: 2025年10月29日