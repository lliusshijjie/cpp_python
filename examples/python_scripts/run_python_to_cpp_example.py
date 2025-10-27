import sys
import os

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'Debug'))

import example_cpp_lib_module

# Test MathCalculator
calc = example_cpp_lib_module.MathCalculator()
print(f"1 + 2 = {calc.add(1, 2)}")

# Test StringProcessor
processor = example_cpp_lib_module.StringProcessor()
print(f"'abc' + 'def' = {processor.concatenate('abc', 'def')}")

# Test DataContainer
container = example_cpp_lib_module.DataContainer()
container.add_data("a", 1)
print(f"container.get_data('a') = {container.get_data('a')}")

# Test CallbackExample
def py_callback(s):
    print(f"Python callback called with: {s}")

callback_example = example_cpp_lib_module.CallbackExample()
callback_example.execute_callback(py_callback)

# Test global function
print(f"global_function_example('test') = {example_cpp_lib_module.global_function_example('test')}")