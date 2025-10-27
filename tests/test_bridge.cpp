#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <filesystem>
#include "python_bridge.h"
#include "type_converter.h"
#include "error_handler.h"

class TestRunner
{
private:
    int total_tests = 0;
    int passed_tests = 0;

public:
    void runTest(const std::string &test_name, std::function<void()> test_func)
    {
        total_tests++;
        std::cout << "Running test: " << test_name << "... ";

        try
        {
            test_func();
            passed_tests++;
            std::cout << "PASSED" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "FAILED - " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cout << "FAILED - Unknown exception" << std::endl;
        }
    }

    void printSummary()
    {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << total_tests << std::endl;
        std::cout << "Passed: " << passed_tests << std::endl;
        std::cout << "Failed: " << (total_tests - passed_tests) << std::endl;
        if (total_tests > 0) {
            std::cout << "Success rate: " << (100.0 * passed_tests / total_tests) << "%" << std::endl;
        }
    }
};

void testPythonInterpreter()
{
    auto &interpreter = cpppy_bridge::PythonInterpreter::getInstance();

    // Test initialization
    assert(!interpreter.isInitialized());
    interpreter.initialize();
    assert(interpreter.isInitialized());

    // Test code execution
    py::object result = interpreter.execute("2 + 3");
    assert(result.cast<int>() == 5);

    // Test adding module path
    interpreter.addModulePath("./test_path");

    std::cout << "PythonInterpreter tests passed" << std::endl;
}

void testPythonModule()
{
    // Create a test Python file
    std::string test_module_content = R"(
def test_function(x, y):
    return x + y

def test_list_function(lst):
    return sum(lst)

test_variable = 42
)";

    // Write to a temporary file
    std::ofstream temp_file("test_module.py");
    temp_file << test_module_content;
    temp_file.close();

    try
    {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize();
        auto module = bridge.loadModule("test_module");
        assert(module->isLoaded());

        // Test function existence check
        assert(module->hasFunction("test_function"));
        assert(!module->hasFunction("nonexistent_function"));

        // Test function call
        int result = module->callFunction<int>("test_function", 10, 20);
        assert(result == 30);

        // Test list argument
        std::vector<int> test_list = {1, 2, 3, 4, 5};
        int sum_result = module->callFunction<int>("test_list_function", test_list);
        assert(sum_result == 15);

        // Test attribute access
        py::object var = module->getAttribute("test_variable");
        assert(var.cast<int>() == 42);

        // Test attribute setting
        module->setAttribute("test_variable", py::int_(100));
        py::object new_var = module->getAttribute("test_variable");
        assert(new_var.cast<int>() == 100);

        std::cout << "PythonModule tests passed" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "PythonModule test failed: " << e.what() << std::endl;
        std::remove("test_module.py");
        throw;
    }

    // Clean up temporary file
    std::remove("test_module.py");
}

void testPythonFunction()
{
    // Create test module
    std::string test_content = R"(
def multiply(a, b):
    return a * b

def greet(name):
    return f"Hello, {name}!"
)";

    std::ofstream temp_file("test_func_module.py");
    temp_file << test_content;
    temp_file.close();

    try
    {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize();
        auto module = bridge.loadModule("test_func_module");
        cpppy_bridge::PythonFunction multiply_func(module, "multiply");
        cpppy_bridge::PythonFunction greet_func(module, "greet");

        assert(multiply_func.isValid());
        assert(greet_func.isValid());

        // Test numeric function
        double result = multiply_func.call<double>(3.5, 2.0);
        assert(std::abs(result - 7.0) < 0.001);

        // Test string function
        std::string greeting = greet_func.call<std::string>("World");
        assert(greeting == "Hello, World!");

        std::cout << "PythonFunction tests passed" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "PythonFunction test failed: " << e.what() << std::endl;
        std::remove("test_func_module.py");
        throw;
    }

    std::remove("test_func_module.py");
}

void testPythonBridge()
{
    cpppy_bridge::PythonBridge bridge;

    // Test initialization
    assert(bridge.initialize());

    // Create test module
    std::string test_content = R"(
def add_numbers(a, b):
    return a + b

def process_dict(data):
    result = {}
    for key, value in data.items():
        result[key] = value * 2
    return result
)";

    std::ofstream temp_file("bridge_test_module.py");
    temp_file << test_content;
    temp_file.close();

    try
    {
        // Test module loading
        auto module = bridge.loadModule("bridge_test_module");
        assert(module != nullptr);
        assert(module->isLoaded());

        // Test function creation
        auto func = bridge.createFunction("bridge_test_module", "add_numbers");
        assert(func != nullptr);
        assert(func->isValid());

        // Test function call
        double result = func->call<double>(10.5, 20.3);
        assert(std::abs(result - 30.8) < 0.001);

        // Test code execution
        py::object code_result = bridge.executeCode("3 * 4");
        assert(code_result.cast<int>() == 12);

        std::cout << "PythonBridge tests passed" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "PythonBridge test failed: " << e.what() << std::endl;
        std::remove("bridge_test_module.py");
        throw;
    }

    std::remove("bridge_test_module.py");
}

void testTypeConverter()
{
    cpppy_bridge::PythonBridge bridge;
    bridge.initialize();
    // Test basic type conversions
    py::object py_int = cpppy_bridge::TypeConverter::toPython(42);
    py::object py_double = cpppy_bridge::TypeConverter::toPython(3.14);
    py::object py_string = cpppy_bridge::TypeConverter::toPython(std::string("Hello"));
    py::object py_bool = cpppy_bridge::TypeConverter::toPython(true);

    // Python -> C++
    int cpp_int = cpppy_bridge::TypeConverter::fromPython<int>(py_int);
    double cpp_double = cpppy_bridge::TypeConverter::fromPython<double>(py_double);
    std::string cpp_string = cpppy_bridge::TypeConverter::fromPython<std::string>(py_string);
    bool cpp_bool = cpppy_bridge::TypeConverter::fromPython<bool>(py_bool);

    assert(cpp_int == 42);
    assert(std::abs(cpp_double - 3.14) < 0.001);
    assert(cpp_string == "Hello");
    assert(cpp_bool == true);

    // Test type checking
    assert(cpppy_bridge::TypeConverter::canConvert<int>(py_int));
    assert(cpppy_bridge::TypeConverter::canConvert<double>(py_double));
    assert(cpppy_bridge::TypeConverter::canConvert<std::string>(py_string));
    assert(cpppy_bridge::TypeConverter::canConvert<bool>(py_bool));

    // Test safe conversion
    auto safe_int = cpppy_bridge::TypeConverter::safeCast<int>(py_int);
    assert(safe_int.has_value());
    assert(safe_int.value() == 42);

    auto safe_fail = cpppy_bridge::TypeConverter::safeCast<int>(py_string);
    assert(!safe_fail.has_value());

    std::cout << "TypeConverter tests passed" << std::endl;
}

void testErrorHandling()
{
    // Create a test module that generates errors
    std::string error_module_content = R"(
def divide_by_zero():
    return 1 / 0

def type_error():
    return "string" + 42

def value_error():
    raise ValueError("Test value error")
)";

    std::ofstream temp_file("error_test_module.py");
    temp_file << error_module_content;
    temp_file.close();

    try
    {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize();
        auto module = bridge.loadModule("error_test_module");
        assert(module->isLoaded());

        // Test error callback
        bool callback_triggered = false;
        cpppy_bridge::ErrorHandler::addErrorCallback([&callback_triggered](const cpppy_bridge::PythonErrorInfo &info)
                                                     {
            callback_triggered = true;
            assert(!info.type.empty());
            assert(!info.message.empty()); });

        // Test zero division error
        try
        {
            module->callFunction<void>("divide_by_zero");
            assert(false); // Should not be reached
        }
        catch (const cpppy_bridge::PythonBridgeException &)
        {
            // Expected exception
        }

        // Test type error
        try
        {
            module->callFunction<void>("type_error");
            assert(false); // Should not be reached
        }
        catch (const cpppy_bridge::PythonBridgeException &)
        {
            // Expected exception
        }

        // Test value error
        try
        {
            module->callFunction<void>("value_error");
            assert(false); // Should not be reached
        }
        catch (const cpppy_bridge::PythonBridgeException &)
        {
            // Expected exception
        }

        assert(callback_triggered);

        // Test safe execution
        auto safe_result = cpppy_bridge::ErrorHandler::safeExecuteOptional([&]()
                                                                           { return module->callFunction<double>("divide_by_zero"); });
        assert(!safe_result.has_value());

        std::cout << "ErrorHandling tests passed" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ErrorHandling test failed: " << e.what() << std::endl;
        std::remove("error_test_module.py");
        throw;
    }

    std::remove("error_test_module.py");
}

void testComplexDataTypes()
{
    // Create test module
    std::string complex_module_content = R"(
def process_vector(vec):
    return [x * 2 for x in vec]

def process_map(data):
    result = {}
    for key, value in data.items():
        result[key] = value + 10
    return result

def return_tuple():
    return (1, 2, 3)

def process_nested(data):
    result = {}
    for key, values in data.items():
        result[key] = sum(values)
    return result
)";

    std::ofstream temp_file("complex_test_module.py");
    temp_file << complex_module_content;
    temp_file.close();

    try
    {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize();
        auto module = bridge.loadModule("complex_test_module");
        assert(module->isLoaded());

        // Test vector processing
        std::vector<int> input_vec = {1, 2, 3, 4, 5};
        auto result_vec = module->callFunction<std::vector<int>>("process_vector", input_vec);
        assert(result_vec.size() == 5);
        for (size_t i = 0; i < result_vec.size(); ++i)
        {
            assert(result_vec[i] == input_vec[i] * 2);
        }

        // Test map processing
        std::map<std::string, int> input_map = {{"a", 1}, {"b", 2}, {"c", 3}};
        auto result_map = module->callFunction<std::map<std::string, int>>("process_map", input_map);
        assert(result_map.size() == 3);
        assert(result_map["a"] == 11);
        assert(result_map["b"] == 12);
        assert(result_map["c"] == 13);

        // Test tuple return
        auto tuple_result = module->callFunction<std::tuple<int, int, int>>("return_tuple");
        assert(std::get<0>(tuple_result) == 1);
        assert(std::get<1>(tuple_result) == 2);
        assert(std::get<2>(tuple_result) == 3);

        // Test nested data structures
        std::map<std::string, std::vector<int>> nested_data = {
            {"group1", {1, 2, 3}},
            {"group2", {4, 5, 6, 7}},
            {"group3", {8, 9}}};
        auto nested_result = module->callFunction<std::map<std::string, int>>("process_nested", nested_data);
        assert(nested_result["group1"] == 6);
        assert(nested_result["group2"] == 22);
        assert(nested_result["group3"] == 17);

        std::cout << "ComplexDataTypes tests passed" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ComplexDataTypes test failed: " << e.what() << std::endl;
        std::remove("complex_test_module.py");
        throw;
    }

    std::remove("complex_test_module.py");
}

int main()
{
    std::cout << "C++ Python Bridge Test Suite" << std::endl;
    std::cout << "=============================" << std::endl;

    TestRunner runner;

    // Run all tests
    runner.runTest("PythonInterpreter", testPythonInterpreter);
    runner.runTest("PythonModule", testPythonModule);
    runner.runTest("PythonFunction", testPythonFunction);
    runner.runTest("PythonBridge", testPythonBridge);
    runner.runTest("TypeConverter", testTypeConverter);
    runner.runTest("ErrorHandling", testErrorHandling);
    runner.runTest("ComplexDataTypes", testComplexDataTypes);

    // Print test summary
    runner.printSummary();

    return 0;
}