# API 参考文档

## 概述

本文档详细描述了C++ Python Bridge库的所有API接口。该库使用 `cpppy_bridge` 命名空间来避免与标准库的命名冲突。

## 命名空间

所有的类和函数都在 `cpppy_bridge` 命名空间中。

```cpp
using namespace cpppy_bridge;
// 或者使用完整命名空间
cpppy_bridge::PythonBridge bridge;
```

## 核心类

### PythonInterpreter

Python解释器管理类，负责Python运行时环境的初始化和管理。采用单例模式。

#### 类定义

```cpp
class PythonInterpreter {
public:
    static PythonInterpreter& getInstance();
    
    void initialize();
    void finalize();
    bool isInitialized() const;
    
    void addModulePath(const std::string& path);
    py::object execute(const std::string& code);
    
private:
    PythonInterpreter() = default;
    ~PythonInterpreter();
    
    bool m_initialized = false;
    std::unique_ptr<py::scoped_interpreter> m_interpreter;
};
```

#### 方法详解

##### `static PythonInterpreter& getInstance()`

获取解释器单例实例。

**返回值**: PythonInterpreter的引用

**示例**:
```cpp
auto& interpreter = PythonInterpreter::getInstance();
```

##### `void initialize()`

初始化Python解释器。

**异常**: 
- `PythonInterpreterException` - 初始化失败时抛出

**示例**:
```cpp
auto& interpreter = PythonInterpreter::getInstance();
interpreter.initialize();
```

##### `void finalize()`

终止Python解释器。

**示例**:
```cpp
interpreter.finalize();
```

##### `bool isInitialized() const`

检查解释器是否已初始化。

**返回值**: `true` 如果已初始化，否则 `false`

##### `void addModulePath(const std::string& path)`

添加Python模块搜索路径。

**参数**:
- `path` - 要添加的路径

**异常**:
- `std::runtime_error` - 解释器未初始化时抛出

**示例**:
```cpp
interpreter.addModulePath("./python_scripts");
interpreter.addModulePath("/usr/local/lib/python_modules");
```

##### `py::object execute(const std::string& code)`

执行Python代码。

**参数**:
- `code` - 要执行的Python代码字符串

**返回值**: 执行结果的Python对象

**异常**:
- `PythonInterpreterException` - 代码执行失败时抛出

**示例**:
```cpp
auto result = interpreter.execute("2 + 3");
int value = result.cast<int>(); // value = 5
```

---

### PythonModule

Python模块封装类，提供高级接口来加载和调用Python模块。

#### 类定义

```cpp
class PythonModule {
public:
    explicit PythonModule(const std::string& module_name);
    ~PythonModule() = default;
    
    bool isLoaded() const;
    bool hasFunction(const std::string& func_name) const;
    
    template<typename ReturnType, typename... Args>
    ReturnType callFunction(const std::string& func_name, Args&&... args);
    
    py::object callFunction(const std::string& func_name, const std::vector<py::object>& args = {});
    
    py::object getAttribute(const std::string& attr_name);
    void setAttribute(const std::string& attr_name, const py::object& value);
    
private:
    std::string m_module_name;
    py::module m_module;
    bool m_loaded = false;
};
```

#### 方法详解

##### `explicit PythonModule(const std::string& module_name)`

构造函数，加载指定的Python模块。

**参数**:
- `module_name` - Python模块名称

**异常**:
- `PythonModuleException` - 模块加载失败时抛出

**示例**:
```cpp
PythonModule math_module("math_operations");
```

##### `bool isLoaded() const`

检查模块是否成功加载。

**返回值**: `true` 如果模块已加载，否则 `false`

##### `bool hasFunction(const std::string& func_name) const`

检查模块中是否存在指定函数。

**参数**:
- `func_name` - 函数名称

**返回值**: `true` 如果函数存在，否则 `false`

**示例**:
```cpp
if (math_module.hasFunction("add")) {
    // 函数存在，可以安全调用
}
```

##### `template<typename ReturnType, typename... Args> ReturnType callFunction(const std::string& func_name, Args&&... args)`

调用Python函数（模板版本）。

**模板参数**:
- `ReturnType` - 返回值类型
- `Args...` - 参数类型包

**参数**:
- `func_name` - 函数名称
- `args...` - 函数参数

**返回值**: 指定类型的返回值

**异常**:
- `PythonFunctionException` - 函数调用失败时抛出
- `TypeConversionException` - 类型转换失败时抛出

**示例**:
```cpp
// 调用返回int的函数
int result = math_module.callFunction<int>("add", 10, 20);

// 调用返回vector的函数
std::vector<double> numbers = {1.0, 2.0, 3.0};
double sum = math_module.callFunction<double>("sum_list", numbers);

// 调用返回复杂类型的函数
auto matrix = math_module.callFunction<std::vector<std::vector<double>>>(
    "create_matrix", 3, 3);
```

##### `py::object callFunction(const std::string& func_name, const std::vector<py::object>& args = {})`

调用Python函数（返回py::object）。

**参数**:
- `func_name` - 函数名称
- `args` - Python对象参数列表

**返回值**: Python对象

**示例**:
```cpp
std::vector<py::object> args = {py::cast(10), py::cast(20)};
py::object result = math_module.callFunction("add", args);
```

##### `py::object getAttribute(const std::string& attr_name)`

获取模块属性。

**参数**:
- `attr_name` - 属性名称

**返回值**: 属性值的Python对象

**异常**:
- `PythonModuleException` - 属性不存在时抛出

**示例**:
```cpp
py::object pi_value = math_module.getAttribute("PI");
double pi = pi_value.cast<double>();
```

##### `void setAttribute(const std::string& attr_name, const py::object& value)`

设置模块属性。

**参数**:
- `attr_name` - 属性名称
- `value` - 属性值

**示例**:
```cpp
math_module.setAttribute("DEBUG_MODE", py::cast(true));
```

---

### PythonFunction

Python函数封装类，提供更便捷的函数调用接口。

#### 类定义

```cpp
class PythonFunction {
public:
    PythonFunction(const std::string& module_name, const std::string& func_name);
    PythonFunction(std::shared_ptr<PythonModule> module, const std::string& func_name);
    
    bool isValid() const;
    
    template<typename ReturnType, typename... Args>
    ReturnType operator()(Args&&... args);
    
    template<typename ReturnType, typename... Args>
    ReturnType call(Args&&... args);

    py::object call(const std::vector<py::object>& args = {});
    
private:
    std::shared_ptr<PythonModule> m_module;
    std::string m_func_name;
    py::object m_function;
    bool m_valid = false;
};
```

#### 方法详解

##### `PythonFunction(const std::string& module_name, const std::string& func_name)`

构造函数，从模块名和函数名创建函数对象。

**参数**:
- `module_name` - Python模块名称
- `func_name` - 函数名称

**示例**:
```cpp
PythonFunction add_func("math_operations", "add");
```

##### `PythonFunction(std::shared_ptr<PythonModule> module, const std::string& func_name)`

构造函数，从模块对象和函数名创建函数对象。

**参数**:
- `module` - PythonModule共享指针
- `func_name` - 函数名称

**示例**:
```cpp
auto module = std::make_shared<PythonModule>("math_operations");
PythonFunction add_func(module, "add");
```

##### `bool isValid() const`

检查函数是否有效可用。

**返回值**: `true` 如果函数有效，否则 `false`

##### `template<typename ReturnType, typename... Args> ReturnType operator()(Args&&... args)`

函数调用操作符。

**模板参数**:
- `ReturnType` - 返回值类型
- `Args...` - 参数类型包

**参数**:
- `args...` - 函数参数

**返回值**: 指定类型的返回值

**示例**:
```cpp
PythonFunction add_func("math_operations", "add");
int result = add_func.operator()<int>(10, 20);
// 或者简化为
int result = add_func(10, 20); // 需要显式指定返回类型
```

##### `template<typename ReturnType, typename... Args> ReturnType call(Args&&... args)`

调用函数。

**示例**:
```cpp
double result = add_func.call<double>(10.5, 20.3);
```

---

### PythonBridge

高级桥接管理类，提供完整的Python集成解决方案。

#### 类定义

```cpp
class PythonBridge {
public:
    PythonBridge();
    ~PythonBridge();
    
    bool initialize(const std::vector<std::string>& module_paths = {});
    
    std::shared_ptr<PythonModule> loadModule(const std::string& module_name);
    std::shared_ptr<PythonFunction> createFunction(const std::string& module_name, 
                                                   const std::string& func_name);
    
    py::object executeFile(const std::string& file_path);
    py::object executeCode(const std::string& code);
    
    std::shared_ptr<PythonModule> getModule(const std::string& module_name);
    
private:
    std::map<std::string, std::shared_ptr<PythonModule>> m_modules;
    bool m_initialized = false;
};
```

#### 方法详解

##### `bool initialize(const std::vector<std::string>& module_paths = {})`

初始化桥接器。

**参数**:
- `module_paths` - Python模块搜索路径列表

**返回值**: `true` 如果初始化成功，否则 `false`

**示例**:
```cpp
PythonBridge bridge;
bridge.initialize({"./python_scripts", "/usr/local/lib/python_modules"});
```

##### `std::shared_ptr<PythonModule> loadModule(const std::string& module_name)`

加载Python模块。

**参数**:
- `module_name` - 模块名称

**返回值**: PythonModule的共享指针，失败时返回nullptr

**示例**:
```cpp
auto module = bridge.loadModule("math_operations");
if (module) {
    // 模块加载成功
}
```

##### `std::shared_ptr<PythonFunction> createFunction(const std::string& module_name, const std::string& func_name)`

创建函数对象。

**参数**:
- `module_name` - 模块名称
- `func_name` - 函数名称

**返回值**: PythonFunction的共享指针

**示例**:
```cpp
auto func = bridge.createFunction("math_operations", "add");
int result = func->call<int>(10, 20);
```

##### `py::object executeFile(const std::string& file_path)`

执行Python文件。

**参数**:
- `file_path` - Python文件路径

**返回值**: 执行结果的Python对象

**示例**:
```cpp
auto result = bridge.executeFile("./scripts/setup.py");
```

##### `py::object executeCode(const std::string& code)`

执行Python代码。

**参数**:
- `code` - Python代码字符串

**返回值**: 执行结果的Python对象

**示例**:
```cpp
auto result = bridge.executeCode("import math; math.sqrt(16)");
double value = result.cast<double>(); // value = 4.0
```

---

## 类型转换系统

### TypeConverter

类型转换工具类，提供C++和Python之间的类型转换功能。

#### 类定义

```cpp
class TypeConverter {
public:
    template<typename T>
    static py::object toPython(const T& value);
    
    template<typename T>
    static T fromPython(const py::object& obj);
    
    template<typename T>
    static bool canConvert(const py::object& obj);
    
    static std::string getPythonType(const py::object& obj);
    
    template<typename T>
    static std::optional<T> safeCast(const py::object& obj);
};
```

#### 方法详解

##### `template<typename T> static py::object toPython(const T& value)`

将C++值转换为Python对象。

**模板参数**:
- `T` - C++类型

**参数**:
- `value` - 要转换的C++值

**返回值**: Python对象

**示例**:
```cpp
py::object py_int = TypeConverter::toPython(42);
py::object py_string = TypeConverter::toPython(std::string("hello"));
py::object py_vector = TypeConverter::toPython(std::vector<int>{1, 2, 3});
```

##### `template<typename T> static T fromPython(const py::object& obj)`

将Python对象转换为C++值。

**模板参数**:
- `T` - 目标C++类型

**参数**:
- `obj` - Python对象

**返回值**: C++值

**异常**:
- `TypeConversionException` - 转换失败时抛出

**示例**:
```cpp
py::object py_obj = py::cast(42);
int cpp_int = TypeConverter::fromPython<int>(py_obj);
```

##### `template<typename T> static bool canConvert(const py::object& obj)`

检查Python对象是否可以转换为指定的C++类型。

**模板参数**:
- `T` - 目标C++类型

**参数**:
- `obj` - Python对象

**返回值**: `true` 如果可以转换，否则 `false`

**示例**:
```cpp
if (TypeConverter::canConvert<int>(py_obj)) {
    int value = TypeConverter::fromPython<int>(py_obj);
}
```

##### `static std::string getPythonType(const py::object& obj)`

获取Python对象的类型名称。

**参数**:
- `obj` - Python对象

**返回值**: 类型名称字符串

**示例**:
```cpp
std::string type_name = TypeConverter::getPythonType(py_obj);
std::cout << "Type: " << type_name << std::endl;
```

##### `template<typename T> static std::optional<T> safeCast(const py::object& obj)`

安全转换Python对象为C++值。

**模板参数**:
- `T` - 目标C++类型

**参数**:
- `obj` - Python对象

**返回值**: `std::optional<T>`，转换成功时包含值，失败时为空

**示例**:
```cpp
auto result = TypeConverter::safeCast<int>(py_obj);
if (result.has_value()) {
    int value = result.value();
} else {
    std::cout << "转换失败" << std::endl;
}
```

### 支持的类型

#### 基本类型
- `int`, `long`, `float`, `double`
- `bool`
- `std::string`

#### 容器类型
- `std::vector<T>`
- `std::map<K, V>`
- `std::set<T>`
- `std::tuple<...>`
- `std::pair<T1, T2>`

#### 特殊类型模板特化

```cpp
// 字符串特化
template<> py::object TypeConverter::toPython<std::string>(const std::string& value);
template<> std::string TypeConverter::fromPython<std::string>(const py::object& obj);

// 数值类型特化
template<> py::object TypeConverter::toPython<int>(const int& value);
template<> int TypeConverter::fromPython<int>(const py::object& obj);

template<> py::object TypeConverter::toPython<double>(const double& value);
template<> double TypeConverter::fromPython<double>(const py::object& obj);

// 布尔类型特化
template<> py::object TypeConverter::toPython<bool>(const bool& value);
template<> bool TypeConverter::fromPython<bool>(const py::object& obj);
```

---

### ComplexTypeConverter

复杂数据结构转换器。

#### 类定义

```cpp
class ComplexTypeConverter {
public:
    template<typename T>
    static py::object vectorToPython(const std::vector<T>& vec);
    
    template<typename T>
    static std::vector<T> vectorFromPython(const py::object& obj);
    
    template<typename K, typename V>
    static py::object mapToPython(const std::map<K, V>& map);
    
    template<typename K, typename V>
    static std::map<K, V> mapFromPython(const py::object& obj);
    
    // 其他复杂类型转换方法...
};
```

### NumpyConverter

NumPy数组转换器。

#### 类定义

```cpp
class NumpyConverter {
public:
    template<typename T>
    static py::array_t<T> vectorToNumpy(const std::vector<T>& vec);
    
    template<typename T>
    static std::vector<T> numpyToVector(const py::array_t<T>& arr);
    
    template<typename T>
    static py::array_t<T> matrix2DToNumpy(const std::vector<std::vector<T>>& matrix);
    
    template<typename T>
    static std::vector<std::vector<T>> numpyToMatrix2D(const py::array_t<T>& arr);
    
    static std::vector<size_t> getArrayShape(const py::array& arr);
    static std::string getArrayDtype(const py::array& arr);
    static size_t getArraySize(const py::array& arr);
};
```

#### 示例

```cpp
// C++ vector 转 NumPy数组
std::vector<double> data = {1.0, 2.0, 3.0, 4.0};
auto numpy_array = NumpyConverter::vectorToNumpy(data);

// NumPy数组转 C++ vector
auto cpp_vector = NumpyConverter::numpyToVector<double>(numpy_array);

// 2D矩阵转换
std::vector<std::vector<double>> matrix = {{1, 2}, {3, 4}};
auto numpy_matrix = NumpyConverter::matrix2DToNumpy(matrix);
```

---

## 错误处理系统

### ErrorHandler

错误处理工具类，提供异常捕获和错误回调功能。

#### 类定义

```cpp
class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const PythonErrorInfo&)>;
    
    static void setGlobalErrorCallback(ErrorCallback callback);
    static void addErrorCallback(ErrorCallback callback);
    static void clearErrorCallbacks();
    
    static PythonErrorInfo handlePythonException(const py::error_already_set& e);
    static void convertPythonException(const py::error_already_set& e);
    
    template<typename Func>
    static auto safeExecute(Func&& func) -> decltype(func());
    
    template<typename Func>
    static auto safeExecuteOptional(Func&& func) -> std::optional<decltype(func())>;
    
    static void setVerboseErrors(bool verbose);
    static void setErrorLogging(bool enable);
};
```

#### 方法详解

##### `static void addErrorCallback(ErrorCallback callback)`

添加错误回调函数。

**参数**:
- `callback` - 错误回调函数

**示例**:
```cpp
ErrorHandler::addErrorCallback([](const PythonErrorInfo& error) {
    std::cout << "Python Error: " << error.message << std::endl;
    std::cout << "Type: " << error.type << std::endl;
    if (!error.traceback.empty()) {
        std::cout << "Traceback: " << error.traceback << std::endl;
    }
});
```

##### `template<typename Func> static auto safeExecute(Func&& func) -> decltype(func())`

安全执行函数，捕获并处理异常。

**模板参数**:
- `Func` - 函数类型

**参数**:
- `func` - 要执行的函数

**返回值**: 函数的返回值

**异常**: 重新抛出转换后的异常

**示例**:
```cpp
auto result = ErrorHandler::safeExecute([&]() {
    return module->callFunction<double>("risky_function", 10);
});
```

##### `template<typename Func> static auto safeExecuteOptional(Func&& func) -> std::optional<decltype(func())>`

安全执行函数，返回optional结果。

**模板参数**:
- `Func` - 函数类型

**参数**:
- `func` - 要执行的函数

**返回值**: `std::optional`，成功时包含结果，失败时为空

**示例**:
```cpp
auto result = ErrorHandler::safeExecuteOptional([&]() {
    return module->callFunction<double>("risky_function", 10);
});

if (result.has_value()) {
    std::cout << "Result: " << result.value() << std::endl;
} else {
    std::cout << "Function execution failed" << std::endl;
}
```

### 异常类型

#### PythonBridgeException

所有Python桥接异常的基类。

```cpp
class PythonBridgeException : public std::exception {
public:
    explicit PythonBridgeException(const std::string& message);
    const char* what() const noexcept override;
};
```

#### PythonInterpreterException

Python解释器相关异常。

```cpp
class PythonInterpreterException : public PythonBridgeException {
public:
    explicit PythonInterpreterException(const std::string& message);
};
```

#### PythonModuleException

Python模块相关异常。

```cpp
class PythonModuleException : public PythonBridgeException {
public:
    PythonModuleException(const std::string& module_name, const std::string& message);
    const std::string& getModuleName() const;
};
```

#### PythonFunctionException

Python函数调用相关异常。

```cpp
class PythonFunctionException : public PythonBridgeException {
public:
    PythonFunctionException(const std::string& function_name, const std::string& message);
    const std::string& getFunctionName() const;
};
```

#### TypeConversionException

类型转换相关异常。

```cpp
class TypeConversionException : public PythonBridgeException {
public:
    TypeConversionException(const std::string& from_type, const std::string& to_type, 
                           const std::string& message);
    const std::string& getFromType() const;
    const std::string& getToType() const;
};
```

### PythonErrorInfo

Python错误信息结构。

```cpp
struct PythonErrorInfo {
    std::string type;           // 异常类型
    std::string message;        // 错误消息
    std::string traceback;      // 堆栈跟踪
    std::string file;           // 文件名
    int line = -1;              // 行号
    std::string function;       // 函数名
};
```

---

## 自定义类型注册

### CustomTypeRegistry

自定义类型转换器注册表。

#### 类定义

```cpp
class CustomTypeRegistry {
public:
    template<typename CppType>
    static void registerToPython(std::function<py::object(const CppType&)> converter);
    
    template<typename CppType>
    static void registerFromPython(std::function<CppType(const py::object&)> converter);
    
    template<typename CppType>
    static bool hasToPythonConverter();
    
    template<typename CppType>
    static bool hasFromPythonConverter();
    
    template<typename CppType>
    static py::object convertToPython(const CppType& value);
    
    template<typename CppType>
    static CppType convertFromPython(const py::object& obj);
};
```

#### 宏定义

```cpp
#define REGISTER_CUSTOM_TYPE_CONVERTER(CppType, ToPythonFunc, FromPythonFunc) \
    do { \
        CustomTypeRegistry::registerToPython<CppType>(ToPythonFunc); \
        CustomTypeRegistry::registerFromPython<CppType>(FromPythonFunc); \
    } while(0)
```

#### 使用示例

```cpp
// 定义自定义类型
struct Point {
    double x, y;
};

// 注册转换器
REGISTER_CUSTOM_TYPE_CONVERTER(
    Point,
    [](const Point& p) {
        return py::make_tuple(p.x, p.y);
    },
    [](const py::object& obj) {
        auto tuple = obj.cast<py::tuple>();
        Point p;
        p.x = tuple[0].cast<double>();
        p.y = tuple[1].cast<double>();
        return p;
    }
);

// 使用自定义类型
Point p{1.0, 2.0};
auto result = module->callFunction<Point>("process_point", p);
```

---

## 实用宏

### 安全调用宏

```cpp
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
```

#### 使用示例

```cpp
// 使用安全调用宏
auto result = SAFE_PYTHON_CALL(module->callFunction<int>("add", 10, 20));

// 使用可选安全调用宏
auto optional_result = SAFE_PYTHON_CALL_OPTIONAL(
    module->callFunction<double>("risky_function")
);

if (optional_result.has_value()) {
    std::cout << "Result: " << optional_result.value() << std::endl;
}
```

---

## 完整使用示例

### 基本使用流程

```cpp
#include "python_bridge.h"
#include "type_converter.h"
#include "error_handler.h"

int main() {
    try {
        // 1. 设置错误处理
        cpppy_bridge::ErrorHandler::setVerboseErrors(true);
        cpppy_bridge::ErrorHandler::addErrorCallback([](const auto& error) {
            std::cout << "Python Error: " << error.message << std::endl;
        });
        
        // 2. 初始化桥接器
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"./python_scripts"});
        
        // 3. 加载模块
        auto module = bridge.loadModule("math_operations");
        if (!module) {
            std::cerr << "Failed to load module" << std::endl;
            return 1;
        }
        
        // 4. 调用函数
        int result = module->callFunction<int>("add", 10, 20);
        std::cout << "10 + 20 = " << result << std::endl;
        
        // 5. 处理复杂类型
        std::vector<double> numbers = {1.0, 2.0, 3.0, 4.0, 5.0};
        double sum = module->callFunction<double>("sum_list", numbers);
        std::cout << "Sum: " << sum << std::endl;
        
        // 6. 安全执行
        auto safe_result = cpppy_bridge::ErrorHandler::safeExecuteOptional([&]() {
            return module->callFunction<double>("divide", 10.0, 0.0);
        });
        
        if (safe_result.has_value()) {
            std::cout << "Division result: " << safe_result.value() << std::endl;
        } else {
            std::cout << "Division by zero handled safely" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### 高级使用示例

```cpp
// 自定义类型示例
struct Matrix {
    std::vector<std::vector<double>> data;
    size_t rows, cols;
};

// 注册自定义类型转换器
REGISTER_CUSTOM_TYPE_CONVERTER(
    Matrix,
    [](const Matrix& m) {
        py::dict result;
        result["data"] = m.data;
        result["rows"] = m.rows;
        result["cols"] = m.cols;
        return result;
    },
    [](const py::object& obj) {
        auto dict = obj.cast<py::dict>();
        Matrix m;
        m.data = dict["data"].cast<std::vector<std::vector<double>>>();
        m.rows = dict["rows"].cast<size_t>();
        m.cols = dict["cols"].cast<size_t>();
        return m;
    }
);

// 使用自定义类型
Matrix input_matrix{{1, 2}, {3, 4}};
input_matrix.rows = 2;
input_matrix.cols = 2;

auto result_matrix = module->callFunction<Matrix>("process_matrix", input_matrix);
```

---

## 性能优化建议

1. **重用对象**: 避免频繁创建和销毁PythonModule和PythonFunction对象
2. **批量处理**: 对于大量数据，考虑在Python端进行批量处理
3. **类型转换优化**: 使用合适的类型转换方法，避免不必要的转换
4. **错误处理**: 在性能关键路径上使用`safeExecuteOptional`而不是异常处理

## 线程安全性

- PythonInterpreter是线程安全的（单例模式）
- PythonModule和PythonFunction不是线程安全的，需要外部同步
- 建议每个线程使用独立的PythonBridge实例

## 内存管理

- 所有Python对象的生命周期由pybind11自动管理
- C++对象的生命周期需要用户管理
- 使用智能指针管理PythonModule和PythonFunction对象

---

**注意**: 本API使用 `cpppy_bridge` 命名空间来避免与标准库的命名冲突。所有示例代码都基于这个命名空间。