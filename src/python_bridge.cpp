#include "python_bridge.h"
#include <iostream>
#include <filesystem>
#include <stdexcept>

namespace cpppy_bridge {

// PythonInterpreter 实现
PythonInterpreter& PythonInterpreter::getInstance() {
    static PythonInterpreter instance;
    return instance;
}

void PythonInterpreter::initialize() {
    if (m_initialized) {
        return;
    }
    
    try {
        m_interpreter = std::make_unique<py::scoped_interpreter>();
        m_initialized = true;
        
        // 添加当前目录到Python路径
        py::module sys = py::module::import("sys");
        py::list path = sys.attr("path");
        path.append(".");
        
        std::cout << "Python interpreter initialized successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize Python interpreter: " << e.what() << std::endl;
        throw;
    }
}

void PythonInterpreter::finalize() {
    if (m_initialized) {
        m_interpreter.reset();
        m_initialized = false;
        std::cout << "Python interpreter finalized." << std::endl;
    }
}

bool PythonInterpreter::isInitialized() const {
    return m_initialized;
}

void PythonInterpreter::addModulePath(const std::string& path) {
    if (!m_initialized) {
        throw std::runtime_error("Python interpreter not initialized");
    }
    
    try {
        py::module sys = py::module::import("sys");
        py::list sys_path = sys.attr("path");
        
        // 检查路径是否已存在
        bool path_exists = false;
        for (auto item : sys_path) {
            if (item.cast<std::string>() == path) {
                path_exists = true;
                break;
            }
        }
        
        if (!path_exists) {
            sys_path.append(path);
            std::cout << "Added module path: " << path << std::endl;
        }
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Failed to add module path: " + std::string(e.what()));
    }
}

py::object PythonInterpreter::execute(const std::string& code) {
    if (!m_initialized) {
        throw std::runtime_error("Python interpreter not initialized");
    }
    
    try {
        return py::eval(code);
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Failed to execute Python code: " + std::string(e.what()));
    }
}

PythonInterpreter::~PythonInterpreter() {
    finalize();
}

// PythonModule 实现
PythonModule::PythonModule(const std::string& module_name) 
    : m_module_name(module_name) {
    
    auto& interpreter = PythonInterpreter::getInstance();
    if (!interpreter.isInitialized()) {
        interpreter.initialize();
    }
    
    try {
        m_module = py::module::import(module_name.c_str());
        m_loaded = true;
        std::cout << "Successfully loaded module: " << module_name << std::endl;
    } catch (const py::error_already_set& e) {
        std::cerr << "Failed to load module " << module_name << ": " << e.what() << std::endl;
        m_loaded = false;
    }
}

bool PythonModule::isLoaded() const {
    return m_loaded;
}

bool PythonModule::hasFunction(const std::string& func_name) const {
    if (!m_loaded) {
        return false;
    }
    
    try {
        return py::hasattr(m_module, func_name.c_str());
    } catch (const py::error_already_set&) {
        return false;
    }
}

py::object PythonModule::callFunction(const std::string& func_name, const std::vector<py::object>& args) {
    if (!m_loaded) {
        throw std::runtime_error("Module not loaded: " + m_module_name);
    }
    
    try {
        py::object func = m_module.attr(func_name.c_str());
        
        if (args.empty()) {
            return func();
        } else {
            py::tuple py_args = py::cast(args);
            return func(*py_args);
        }
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Python error in " + m_module_name + "." + func_name + ": " + e.what());
    }
}

py::object PythonModule::getAttribute(const std::string& attr_name) {
    if (!m_loaded) {
        throw std::runtime_error("Module not loaded: " + m_module_name);
    }
    
    try {
        return m_module.attr(attr_name.c_str());
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Failed to get attribute " + attr_name + " from " + m_module_name + ": " + e.what());
    }
}

void PythonModule::setAttribute(const std::string& attr_name, const py::object& value) {
    if (!m_loaded) {
        throw std::runtime_error("Module not loaded: " + m_module_name);
    }
    
    try {
        m_module.attr(attr_name.c_str()) = value;
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Failed to set attribute " + attr_name + " in " + m_module_name + ": " + e.what());
    }
}

// PythonFunction 实现
PythonFunction::PythonFunction(const std::string& module_name, const std::string& func_name)
    : m_func_name(func_name) {
    
    m_module = std::make_shared<PythonModule>(module_name);
    
    if (m_module->isLoaded() && m_module->hasFunction(func_name)) {
        try {
            m_function = m_module->getAttribute(func_name);
            m_valid = true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to get function " << func_name << ": " << e.what() << std::endl;
            m_valid = false;
        }
    } else {
        m_valid = false;
    }
}

PythonFunction::PythonFunction(std::shared_ptr<PythonModule> module, const std::string& func_name)
    : m_module(module), m_func_name(func_name) {
    
    if (m_module && m_module->isLoaded() && m_module->hasFunction(func_name)) {
        try {
            m_function = m_module->getAttribute(func_name);
            m_valid = true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to get function " << func_name << ": " << e.what() << std::endl;
            m_valid = false;
        }
    } else {
        m_valid = false;
    }
}

bool PythonFunction::isValid() const {
    return m_valid;
}

py::object PythonFunction::callPy(const std::vector<py::object>& args) {
    if (!m_valid) {
        throw std::runtime_error("Invalid function: " + m_func_name);
    }
    
    try {
        if (args.empty()) {
            return m_function();
        } else {
            py::tuple py_args = py::cast(args);
            py::object result = m_function(*py_args);
            return result;
        }
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Python error in function " + m_func_name + ": " + e.what());
    }
}

// PythonBridge 实现
PythonBridge::PythonBridge() = default;

PythonBridge::~PythonBridge() = default;

bool PythonBridge::initialize(const std::vector<std::string>& module_paths) {
    try {
        auto& interpreter = PythonInterpreter::getInstance();
        interpreter.initialize();
        
        // 添加模块搜索路径
        for (const auto& path : module_paths) {
            interpreter.addModulePath(path);
        }
        
        m_initialized = true;
        std::cout << "PythonBridge initialized successfully." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize PythonBridge: " << e.what() << std::endl;
        m_initialized = false;
        return false;
    }
}

std::shared_ptr<PythonModule> PythonBridge::loadModule(const std::string& module_name) {
    if (!m_initialized) {
        throw std::runtime_error("PythonBridge not initialized");
    }
    
    auto it = m_modules.find(module_name);
    if (it != m_modules.end()) {
        return it->second;
    }
    
    auto module = std::make_shared<PythonModule>(module_name);
    if (module->isLoaded()) {
        m_modules[module_name] = module;
        return module;
    }
    
    return nullptr;
}

std::shared_ptr<PythonFunction> PythonBridge::createFunction(const std::string& module_name, 
                                                             const std::string& func_name) {
    auto module = loadModule(module_name);
    if (!module) {
        return nullptr;
    }
    
    return std::make_shared<PythonFunction>(module, func_name);
}

py::object PythonBridge::executeFile(const std::string& file_path) {
    if (!m_initialized) {
        throw std::runtime_error("PythonBridge not initialized");
    }
    
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("File not found: " + file_path);
    }
    
    try {
        py::eval_file(file_path);
        return py::none();
    } catch (const py::error_already_set& e) {
        throw std::runtime_error("Failed to execute file " + file_path + ": " + e.what());
    }
}

py::object PythonBridge::executeCode(const std::string& code) {
    if (!m_initialized) {
        throw std::runtime_error("PythonBridge not initialized");
    }
    
    auto& interpreter = PythonInterpreter::getInstance();
    return interpreter.execute(code);
}

std::shared_ptr<PythonModule> PythonBridge::getModule(const std::string& module_name) {
    auto it = m_modules.find(module_name);
    return (it != m_modules.end()) ? it->second : nullptr;
}

} // namespace cpppy_bridge