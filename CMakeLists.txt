cmake_minimum_required(VERSION 3.12)
project(CppPythonBridge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Python和pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 QUIET)

if(NOT pybind11_FOUND)
    # 如果没有找到pybind11，从GitHub下载
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${Python3_INCLUDE_DIRS})

# 源文件
set(SOURCES
    src/python_bridge.cpp
    src/type_converter.cpp
    src/error_handler.cpp
)

# 创建静态库
add_library(cpp_python_bridge STATIC ${SOURCES})
target_link_libraries(cpp_python_bridge ${Python3_LIBRARIES} pybind11::embed)

# 创建示例可执行文件
add_executable(example examples/main.cpp)
target_link_libraries(example cpp_python_bridge)

# 创建测试可执行文件
add_executable(test_bridge tests/test_bridge.cpp)
target_link_libraries(test_bridge cpp_python_bridge)

# 设置Python路径
target_compile_definitions(cpp_python_bridge PRIVATE 
    PYTHON_EXECUTABLE="${Python3_EXECUTABLE}")