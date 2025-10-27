import pybind11
import os

cmake_dir = pybind11.get_cmake_dir()
# On Windows, the path might be in mixed format (slashes/backslashes).
# Let's normalize it for CMake.
print(os.path.normpath(cmake_dir).replace('\\', '/'))