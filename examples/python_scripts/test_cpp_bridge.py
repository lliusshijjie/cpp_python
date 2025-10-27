#!/usr/bin/env python3
"""
Example script for calling C++ code from Python.

This script demonstrates how to call C++ functions from Python using the bidirectional bridge.
"""

import sys
import os

def main():
    """Main function."""
    print("Python to C++ Bridge Example")
    print("=" * 50)
    
    try:
        # Add the build directory to the Python path to find the compiled module
        build_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'Debug'))
        sys.path.append(build_dir)
        
        # Import our C++ module
        import example_cpp_lib_module as example_cpp_lib
        print("Successfully imported C++ module")

        test_math_operations(example_cpp_lib)
        test_string_processing(example_cpp_lib)
        test_data_container(example_cpp_lib)
        test_callbacks(example_cpp_lib)
        test_global_functions(example_cpp_lib)
        test_constants(example_cpp_lib)
        demonstrate_bidirectional_bridge(example_cpp_lib)
        
        print("\n All tests completed!")
        
    except ImportError as e:
        print(f"\n Error during import: {e}")
        print(f"Please ensure the C++ extension module is compiled and located in: {build_dir}")
        import traceback
        traceback.print_exc()
    except Exception as e:
        print(f"\n An error occurred during tests: {e}")
        import traceback
        traceback.print_exc()

def test_math_operations(example_cpp_lib):
    """Tests math operations."""
    print("\n Testing Math Operations:")
    
    calc = example_cpp_lib.MathCalculator()
    result = calc.add(3.14, 2.86)
    print(f"  add(3.14, 2.86) = {result}")
    
    result = calc.multiply(5, 7)
    print(f"  multiply(5, 7) = {result}")

def test_string_processing(example_cpp_lib):
    """Tests string processing functions."""
    print("\n Testing String Processing:")
    
    processor = example_cpp_lib.StringProcessor()
    
    print(f"  concatenate('Hello', ' World') = '{processor.concatenate('Hello', ' World')}'")
    print(f"  get_length('Hello World') = {processor.get_length('Hello World')}")

def test_data_container(example_cpp_lib):
    """Tests data container functions."""
    print("\n Testing Data Container:")
    
    container = example_cpp_lib.DataContainer()
    
    container.add_data("item1", 100)
    container.add_data("item2", 200)
    
    print(f"  get_data('item1') = {container.get_data('item1')}")
    print(f"  get_all_data() = {container.get_all_data()}")

def test_callbacks(example_cpp_lib):
    """Tests callback functions."""
    print("\n Testing Callback Functions:")
    
    def python_callback(message):
        print(f"  Python callback received: '{message}'")

    callback_handler = example_cpp_lib.CallbackExample()
    callback_handler.execute_callback(python_callback)

def test_global_functions(example_cpp_lib):
    """Tests global functions."""
    print("\n Testing Global Functions:")
    
    result = example_cpp_lib.global_function_example("some input")
    print(f"  global_function_example('some input') = '{result}'")

def test_constants(example_cpp_lib):
    """Tests constants."""
    print("\n Testing Constants:")
    print("  (No constants are exposed in the current bindings)")

def demonstrate_bidirectional_bridge(example_cpp_lib):
    """Demonstrates bidirectional bridge functionality."""
    print("\n Bidirectional Bridge Demonstration:")
    print("  This example shows bidirectional communication between Python and C++:")
    print("  1. Python calls a C++ function.")
    print("  2. C++ accepts a Python function as a callback.")
    
    def python_processor(text):
        """A text processor in Python."""
        return f"[Python processed] {text.upper()}"
    
    # Use C++ string processing
    processor = example_cpp_lib.StringProcessor()
    cpp_result = processor.concatenate("hello", " world")
    
    # Pass the result to a Python function
    final_result = python_processor(cpp_result)
    
    print(f"  Processing pipeline result: '{final_result}'")
    
    # Use C++ callback mechanism
    print("  Now, C++ will call a Python function:")
    callback_handler = example_cpp_lib.CallbackExample()
    callback_handler.execute_callback(python_processor)

if __name__ == "__main__":
    main()