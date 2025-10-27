# Developer's Guide to the C++ Python Bridge

## Overview

This guide provides a practical, hands-on introduction to using the C++ Python Bridge library. It covers the workflow for calling Python from C++, with detailed examples and best practices.

---

## Calling Python from C++ (C++ -> Python)

This workflow is ideal when you have a C++ application and want to leverage Python for scripting, data analysis, or accessing Python-only libraries.

### Step 1: Initialize the Bridge

The first step is always to initialize the `PythonBridge`. This sets up the embedded interpreter.

```cpp
#include "python_bridge.h"

int main() {
    cpppy_bridge::PythonBridge bridge;
    // Add the directory containing your python scripts to the path
    bridge.initialize({"path/to/your/python_scripts"});
    // ...
}
```

### Step 2: Load a Python Module

Once initialized, you can load any Python module that is in the search path.

```cpp
try {
    auto my_module = bridge.loadModule("my_python_script");
} catch (const cpppy_bridge::PythonModuleException& e) {
    std::cerr << "Failed to load module: " << e.what() << std::endl;
}
```

### Step 3: Call Python Functions

You can call functions from the loaded module and have the results automatically converted back to C++ types.

```cpp
// Python script: my_python_script.py
// def greet(name):
//     return f"Hello, {name}"
// 
// def process_data(data):
//     return sum(data)

std::string message = my_module->callFunction<std::string>("greet", "World");
// message is "Hello, World"

std::vector<double> data = {1.0, 2.5, 3.5};
double sum = my_module->callFunction<double>("process_data", data);
// sum is 7.0
```

### Advanced Usage

- **Error Handling**: Wrap calls in a `try...catch` block to handle Python exceptions that are automatically converted to C++ `PythonBridgeException` types.
- **Data Exchange**: The library seamlessly converts STL containers (`std::vector`, `std::map`, etc.) and can be extended to handle custom C++ structs/classes.

This guide provides the foundation for using the library. For a detailed breakdown of all available functions and classes, refer to the [API Reference](./API_Reference.md).