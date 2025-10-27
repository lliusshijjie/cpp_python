"""
示例Python模块：数学运算
包含各种数学函数供C++调用测试
"""

import math
import numpy as np
from typing import List, Dict, Tuple, Optional


def add(a: float, b: float) -> float:
    """简单加法运算"""
    return a + b


def multiply(a: float, b: float) -> float:
    """简单乘法运算"""
    return a * b


def power(base: float, exponent: float) -> float:
    """幂运算"""
    return math.pow(base, exponent)


def factorial(n: int) -> int:
    """计算阶乘"""
    if n < 0:
        raise ValueError("Factorial is not defined for negative numbers")
    if n == 0 or n == 1:
        return 1
    return math.factorial(n)


def fibonacci(n: int) -> int:
    """计算斐波那契数列第n项"""
    if n < 0:
        raise ValueError("Fibonacci is not defined for negative numbers")
    if n == 0:
        return 0
    if n == 1:
        return 1
    
    a, b = 0, 1
    for _ in range(2, n + 1):
        a, b = b, a + b
    return b


def sum_list(numbers: List[float]) -> float:
    """计算列表元素之和"""
    return sum(numbers)


def average_list(numbers: List[float]) -> float:
    """计算列表元素平均值"""
    if not numbers:
        raise ValueError("Cannot calculate average of empty list")
    return sum(numbers) / len(numbers)


def matrix_multiply(matrix_a: List[List[float]], matrix_b: List[List[float]]) -> List[List[float]]:
    """矩阵乘法"""
    if not matrix_a or not matrix_b:
        raise ValueError("Matrices cannot be empty")
    
    rows_a, cols_a = len(matrix_a), len(matrix_a[0])
    rows_b, cols_b = len(matrix_b), len(matrix_b[0])
    
    if cols_a != rows_b:
        raise ValueError(f"Cannot multiply matrices: {cols_a} != {rows_b}")
    
    result = [[0.0 for _ in range(cols_b)] for _ in range(rows_a)]
    
    for i in range(rows_a):
        for j in range(cols_b):
            for k in range(cols_a):
                result[i][j] += matrix_a[i][k] * matrix_b[k][j]
    
    return result


def numpy_operations(arr: np.ndarray) -> Dict[str, float]:
    """使用NumPy进行数组操作"""
    return {
        "mean": float(np.mean(arr)),
        "std": float(np.std(arr)),
        "min": float(np.min(arr)),
        "max": float(np.max(arr)),
        "sum": float(np.sum(arr))
    }


def process_data(data: Dict[str, List[float]]) -> Dict[str, Dict[str, float]]:
    """处理复杂数据结构"""
    result = {}
    
    for key, values in data.items():
        if not values:
            result[key] = {"error": "empty_list"}
            continue
            
        result[key] = {
            "count": len(values),
            "sum": sum(values),
            "average": sum(values) / len(values),
            "min": min(values),
            "max": max(values)
        }
    
    return result


def get_system_info() -> Dict[str, str]:
    """获取系统信息"""
    import platform
    import sys
    
    return {
        "python_version": sys.version,
        "platform": platform.platform(),
        "architecture": platform.architecture()[0],
        "processor": platform.processor(),
        "python_executable": sys.executable
    }


def simulate_error(error_type: str = "value") -> None:
    """模拟不同类型的错误用于测试异常处理"""
    if error_type == "value":
        raise ValueError("This is a simulated ValueError")
    elif error_type == "type":
        raise TypeError("This is a simulated TypeError")
    elif error_type == "runtime":
        raise RuntimeError("This is a simulated RuntimeError")
    elif error_type == "zero_division":
        return 1 / 0
    else:
        raise Exception(f"Unknown error type: {error_type}")


def optional_return(return_none: bool = False) -> Optional[str]:
    """测试可选返回值"""
    if return_none:
        return None
    return "Hello from Python!"


def tuple_operations(a: int, b: int, c: int) -> Tuple[int, int, int]:
    """返回元组"""
    return (a + b + c, a * b * c, max(a, b, c))


class Calculator:
    """计算器类，演示类的调用"""
    
    def __init__(self, initial_value: float = 0.0):
        self.value = initial_value
        self.history = []
    
    def add(self, x: float) -> float:
        """加法"""
        self.value += x
        self.history.append(f"add({x})")
        return self.value
    
    def subtract(self, x: float) -> float:
        """减法"""
        self.value -= x
        self.history.append(f"subtract({x})")
        return self.value
    
    def multiply(self, x: float) -> float:
        """乘法"""
        self.value *= x
        self.history.append(f"multiply({x})")
        return self.value
    
    def divide(self, x: float) -> float:
        """除法"""
        if x == 0:
            raise ValueError("Cannot divide by zero")
        self.value /= x
        self.history.append(f"divide({x})")
        return self.value
    
    def reset(self) -> None:
        """重置"""
        self.value = 0.0
        self.history.clear()
    
    def get_value(self) -> float:
        """获取当前值"""
        return self.value
    
    def get_history(self) -> List[str]:
        """获取操作历史"""
        return self.history.copy()


# 全局变量用于测试
global_counter = 0


def increment_counter() -> int:
    """增加全局计数器"""
    global global_counter
    global_counter += 1
    return global_counter


def get_counter() -> int:
    """获取全局计数器值"""
    return global_counter


def reset_counter() -> None:
    """重置全局计数器"""
    global global_counter
    global_counter = 0