# Developer's Guide to the C++ Python Bridge

## Overview

This guide provides a practical, hands-on introduction to using the C++ Python Bridge library. It covers the two main workflows: calling Python from C++ and calling C++ from Python, with detailed examples and best practices.

---

## Part 1: Calling Python from C++ (C++ -> Python)

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

---

## Part 2: Calling C++ from Python (Python -> C++)

This workflow is perfect for performance-critical applications where you want to write core logic in C++ and expose it to Python as a high-performance, native module.

### Step 1: Define Your C++ Code

First, write your C++ functions and classes as you normally would.

```cpp
// in cpp_library.h
#include <string>
#include <vector>

namespace my_lib {
    std::string process_text(const std::string& text);

    class DataProcessor {
    public:
        DataProcessor(std::string name);
        void add_item(int item);
        int get_item_count() const;
    private:
        std::string name_;
        std::vector<int> items_;
    };
}
```

### Step 2: Create the Python Bindings

Use the `CPPPY_MODULE` macro and `pybind11` syntax to create the bindings in a `.cpp` file.

```cpp
// in python_bindings.cpp
#include "cpp_module_wrapper.h"
#include "cpp_library.h" // Your C++ library header

CPPPY_MODULE(my_cpp_library, "My C++ library exposed to Python")
{
    // Bind the free function
    m.def("process_text", &my_lib::process_text, "Processes some text.");

    // Bind the DataProcessor class
    pybind11::class_<my_lib::DataProcessor>(m, "DataProcessor")
        .def(pybind11::init<std::string>()) // Bind the constructor
        .def("add_item", &my_lib::DataProcessor::add_item, "Adds an item.")
        .def("get_item_count", &my_lib::DataProcessor::get_item_count, "Gets the item count.");
}
```

### Step 3: Configure CMake

Add the C++ code and the binding file to your `CMakeLists.txt` to create the Python module.

```cmake
# In CMakeLists.txt
pybind11_add_module(my_cpp_library
    src/cpp_library.cpp
    src/python_bindings.cpp
)
```

### Step 4: Use Your C++ Module in Python

After building the project, you can import and use your C++ module in Python just like any other module.

```python
# in your_python_script.py
import my_cpp_library

# Call the C++ function
processed = my_cpp_library.process_text("some data")
print(processed)

# Use the C++ class
processor = my_cpp_library.DataProcessor("My Processor")
processor.add_item(10)
processor.add_item(20)
print(f"Item count: {processor.get_item_count()}") # Prints "Item count: 2"
```

This guide provides the foundation for using the library. For a detailed breakdown of all available functions and classes, refer to the [API Reference](./API_Reference.md).