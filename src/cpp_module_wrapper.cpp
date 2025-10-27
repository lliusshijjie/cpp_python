#include "cpp_module_wrapper.h"
#include <stdexcept>
#include <iostream>

namespace cpppy_bridge {

// CppModuleWrapper implementation
CppModuleWrapper::CppModuleWrapper(const std::string& module_name, const std::string& description)
    : module_name_(module_name), description_(description) {
    // Lazily initialize the module, create it only when needed
}

CppModuleWrapper::~CppModuleWrapper() {}

pybind11::module_ CppModuleWrapper::create_module() {
    // This method should be called within the PYBIND11_MODULE macro
    // Returns a temporary module for testing purposes
    static pybind11::module_ temp_module = pybind11::module_::import("sys");
    return temp_module;
}

void CppModuleWrapper::register_module(bool force_reload) {
    std::cout << "Module '" << module_name_ << "' registration requested." << std::endl;
    // The actual module registration is done via the PYBIND11_MODULE macro
}

// CppFunctionBinder implementation
CppFunctionBinder::CppFunctionBinder() {}

CppFunctionBinder::~CppFunctionBinder() {}

CppFunctionBinder& CppFunctionBinder::doc(const std::string& docstring) {
    docstring_ = docstring;
    return *this;
}

CppFunctionBinder& CppFunctionBinder::arg_names(const std::vector<std::string>& names) {
    arg_names_ = names;
    return *this;
}

// ModuleRegistry implementation
ModuleRegistry& ModuleRegistry::getInstance() {
    static ModuleRegistry instance;
    return instance;
}

void ModuleRegistry::register_module(const std::string& name, 
                                    std::function<void(pybind11::module_&)> init_func) {
    modules_[name] = init_func;
}

std::vector<std::string> ModuleRegistry::get_registered_modules() const {
    std::vector<std::string> names;
    for (const auto& pair : modules_) {
        names.push_back(pair.first);
    }
    return names;
}

void ModuleRegistry::initialize_all_modules() {
    try {
        for (const auto& pair : modules_) {
            std::cout << "Initializing module: " << pair.first << std::endl;
            // Module initialization logic
        }
    } catch (const std::exception& e) {
        std::cerr << "Error initializing modules: " << e.what() << std::endl;
        throw;
    }
}

} // namespace cpppy_bridge