@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /d C:\Users\10424\Desktop\Project\cpp_call_python\build

cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="D:\Python\Lib\site-packages\pybind11\share\cmake\pybind11" ..

