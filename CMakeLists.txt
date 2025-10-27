cmake_minimum_required(VERSION 3.12)
project(CppPythonBridge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python and pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${Python3_INCLUDE_DIRS})

# Create the C++ to Python example
add_executable(cpp_to_python_example examples/main.cpp src/python_bridge.cpp src/type_converter.cpp src/error_handler.cpp)
target_link_libraries(cpp_to_python_example PRIVATE ${Python3_LIBRARIES} pybind11::embed)

# Create the Python extension module
pybind11_add_module(example_cpp_lib_module SHARED examples/python_bindings.cpp examples/cpp_library.cpp)

# Set Python path for the C++ to Python example
target_compile_definitions(cpp_to_python_example PRIVATE
    PYTHON_EXECUTABLE="${Python3_EXECUTABLE}")