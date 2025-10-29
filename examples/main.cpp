#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include "python_bridge.h"
#include "type_converter.h"
#include "error_handler.h"

void demonstrateBasicFunctions() {
    std::cout << "\n=== Basic Function Call Demonstration ===" << std::endl;
    
    try {
        // Create PythonBridge
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});
        
        // Load math_operations module
        auto math_module = bridge.loadModule("math_operations");
        if (!math_module) {
            std::cerr << "Failed to load math_operations module" << std::endl;
            return;
        }
        
        // Test simple function calls
        std::cout << "Testing basic arithmetic functions:" << std::endl;
        
        // Addition
        double result_add = math_module->callFunction<double>("add", 10.5, 20.3);
        std::cout << "add(10.5, 20.3) = " << result_add << std::endl;
        
        // Multiplication
        double result_mul = math_module->callFunction<double>("multiply", 4.5, 6.2);
        std::cout << "multiply(4.5, 6.2) = " << result_mul << std::endl;
        
        // Power
        double result_pow = math_module->callFunction<double>("power", 2.0, 8.0);
        std::cout << "power(2.0, 8.0) = " << result_pow << std::endl;
        
        // Factorial
        int result_fact = math_module->callFunction<int>("factorial", 5);
        std::cout << "factorial(5) = " << result_fact << std::endl;
        
        // Fibonacci
        int result_fib = math_module->callFunction<int>("fibonacci", 10);
        std::cout << "fibonacci(10) = " << result_fib << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in basic functions demo: " << e.what() << std::endl;
    }
}

void demonstrateContainerOperations() {
    std::cout << "\n=== Container Operations Demonstration ===" << std::endl;
    
    try {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});
        
        auto math_module = bridge.loadModule("math_operations");
        if (!math_module) {
            std::cerr << "Failed to load math_operations module" << std::endl;
            return;
        }
        
        // Test list operations
        std::vector<double> numbers = {1.5, 2.7, 3.9, 4.1, 5.3, 6.8, 7.2};
        std::cout << "Input list: ";
        for (const auto& num : numbers) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
        
        // Calculate sum of list
        double sum_result = math_module->callFunction<double>("sum_list", numbers);
        std::cout << "sum_list() = " << sum_result << std::endl;
        
        // Calculate average
        double avg_result = math_module->callFunction<double>("average_list", numbers);
        std::cout << "average_list() = " << avg_result << std::endl;
        
        // Test matrix multiplication
        std::vector<std::vector<double>> matrix_a = {{1, 2}, {3, 4}};
        std::vector<std::vector<double>> matrix_b = {{5, 6}, {7, 8}};
        
        std::cout << "\nMatrix A: [[1, 2], [3, 4]]" << std::endl;
        std::cout << "Matrix B: [[5, 6], [7, 8]]" << std::endl;
        
        auto result_matrix = math_module->callFunction<std::vector<std::vector<double>>>(
            "matrix_multiply", matrix_a, matrix_b);
        
        std::cout << "Matrix multiplication result:" << std::endl;
        for (const auto& row : result_matrix) {
            std::cout << "[";
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << row[i];
                if (i < row.size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in container operations demo: " << e.what() << std::endl;
    }
}

void demonstrateComplexDataStructures() {
    std::cout << "\n=== Complex Data Structures Demonstration ===" << std::endl;
    
    try {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});
        
        auto math_module = bridge.loadModule("math_operations");
        if (!math_module) {
            std::cerr << "Failed to load math_operations module" << std::endl;
            return;
        }
        
        // Prepare complex data structure
        std::map<std::string, std::vector<double>> data = {
            {"dataset1", {1.0, 2.0, 3.0, 4.0, 5.0}},
            {"dataset2", {10.5, 20.3, 30.7, 40.1}},
            {"dataset3", {100.0, 200.0, 300.0}}
        };
        
        std::cout << "Processing complex data structure..." << std::endl;
        
        // Call Python function to process data
        auto result = math_module->callFunction<py::object>("process_data", data);
        
        // Convert and display the result
        auto processed_data = result.cast<std::map<std::string, std::map<std::string, double>>>();
        
        for (const auto& pair : processed_data) {
            std::cout << "Dataset: " << pair.first << std::endl;
            for (const auto& stats_pair : pair.second) {
                std::cout << "  " << stats_pair.first << ": " << stats_pair.second << std::endl;
            }
        }
        
        // Get system information
        std::cout << "\nSystem Information:" << std::endl;
        py::object sys_info = math_module->callFunction<py::object>("get_system_info");
        auto info_map = sys_info.cast<std::map<std::string, std::string>>();
        
        for (const auto& pair : info_map) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in complex data structures demo: " << e.what() << std::endl;
    }
}

void demonstrateErrorHandling() {
    std::cout << "\n=== Error Handling Demonstration ===" << std::endl;
    
    // Set up an error callback
    cpppy_bridge::ErrorHandler::addErrorCallback([](const cpppy_bridge::PythonErrorInfo& info) {
        std::cout << "Custom error handler triggered:" << std::endl;
        std::cout << "  Type: " << info.type << std::endl;
        std::cout << "  Message: " << info.message << std::endl;
    });
    
    try {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});
        
        auto math_module = bridge.loadModule("math_operations");
        if (!math_module) {
            std::cerr << "Failed to load math_operations module" << std::endl;
            return;
        }
        
        // Test different error types
        std::vector<std::string> error_types = {"value", "type", "runtime", "zero_division"};
        
        for (const auto& error_type : error_types) {
            std::cout << "\nTesting " << error_type << " error:" << std::endl;
            
            try {
                math_module->callFunction<py::object>("simulate_error", error_type);
            } catch (const cpppy_bridge::PythonBridgeException& e) {
                std::cout << "Caught PythonBridgeException: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Caught std::exception: " << e.what() << std::endl;
            }
        }
        
        // Test safe execution
        std::cout << "\nTesting safe execution:" << std::endl;
        auto safe_result = cpppy_bridge::ErrorHandler::safeExecuteOptional([&]() {
            return math_module->callFunction<double>("add", 1.0, 2.0);
        });
        
        if (safe_result) {
            std::cout << "Safe execution result: " << *safe_result << std::endl;
        } else {
            std::cout << "Safe execution failed" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in error handling demo: " << e.what() << std::endl;
    }
}

void demonstrateFunctionWrapper() {
    std::cout << "\n=== Function Wrapper Demonstration ===" << std::endl;
    
    try {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});

        // Create function wrappers
         cpppy_bridge::PythonFunction add_func(bridge.loadModule("math_operations"), "add");
         cpppy_bridge::PythonFunction multiply_func(bridge.loadModule("math_operations"), "multiply");
         cpppy_bridge::PythonFunction fibonacci_func(bridge.loadModule("math_operations"), "fibonacci");
        
        if (!add_func.isValid() || !multiply_func.isValid() || !fibonacci_func.isValid()) {
            std::cerr << "Failed to create function wrappers" << std::endl;
            return;
        }
        
        std::cout << "Using function wrappers:" << std::endl;
        
        // Use function wrappers
        double add_result = add_func.call<double>(15.5, 24.7);
        std::cout << "add_func(15.5, 24.7) = " << add_result << std::endl;
        
        double mul_result = multiply_func.call<double>(3.14, 2.0);
        std::cout << "multiply_func(3.14, 2.0) = " << mul_result << std::endl;
        
        int fib_result = fibonacci_func.call<int>(15);
        std::cout << "fibonacci_func(15) = " << fib_result << std::endl;
        
        // Performance test
        std::cout << "\nPerformance test (1000 calls):" << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000; ++i) {
            add_func.call<double>(i * 0.1, (i + 1) * 0.1);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "1000 function calls took: " << duration.count() << " microseconds" << std::endl;
        std::cout << "Average per call: " << duration.count() / 1000.0 << " microseconds" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in function wrapper demo: " << e.what() << std::endl;
    }
}

void demonstrateGlobalState() {
    std::cout << "\n=== Global State Management Demonstration ===" << std::endl;
    
    try {
        cpppy_bridge::PythonBridge bridge;
        bridge.initialize({"../examples/python_scripts"});
        
        auto math_module = bridge.loadModule("math_operations");
        if (!math_module) {
            std::cerr << "Failed to load math_operations module" << std::endl;
            return;
        }
        
        // Reset counter
        math_module->callFunction<void>("reset_counter");
        
        // Test global state
        std::cout << "Testing global state management:" << std::endl;
        
        for (int i = 0; i < 5; ++i) {
            int counter = math_module->callFunction<int>("increment_counter");
            std::cout << "Counter after increment " << (i + 1) << ": " << counter << std::endl;
        }
        
        int final_counter = math_module->callFunction<int>("get_counter");
        std::cout << "Final counter value: " << final_counter << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in global state demo: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "C++ Python Bridge Demonstration" << std::endl;
    std::cout << "==============================" << std::endl;
    
    try {
        // Set up error handling
         cpppy_bridge::ErrorHandler::setVerboseErrors(true);
         cpppy_bridge::ErrorHandler::setErrorLogging(true);
        
        // Run demonstrations
        demonstrateBasicFunctions();
        demonstrateContainerOperations();
        demonstrateComplexDataStructures();
        demonstrateErrorHandling();
        demonstrateFunctionWrapper();
        demonstrateGlobalState();
        
        std::cout << "\nAll demonstrations completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}