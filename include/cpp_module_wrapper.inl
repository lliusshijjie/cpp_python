#pragma once

namespace cpppy_bridge {

// CppModuleWrapper template method implementation
template<typename Func>
CppModuleWrapper& CppModuleWrapper::bind_function(const std::string& name, Func&& func, 
                                                  const std::string& docstring) {
    if (!module_) {
        throw std::runtime_error("Module not initialized");
    }
    
    // Bind the function using pybind11
    module_->def(name.c_str(), std::forward<Func>(func), docstring.c_str());
    
    // Store function information for later management
    bound_functions_[name] = std::make_any<std::function<void()>>(
        [func]() { /* Additional features like call statistics can be added here */ }
    );
    
    return *this;
}

template<typename Class>
CppClassBinder<Class> CppModuleWrapper::bind_class(const std::string& name, 
                                                   const std::string& docstring) {
    if (!module_) {
        throw std::runtime_error("Module not initialized");
    }
    
    // Create a pybind11 class binding
    auto py_class = pybind11::class_<Class>(*module_, name.c_str(), docstring.c_str());
    
    // Store class information
    bound_classes_[name] = py_class;
    
    return CppClassBinder<Class>(py_class);
}

template<typename T>
CppModuleWrapper& CppModuleWrapper::bind_constant(const std::string& name, const T& value) {
    if (!module_) {
        throw std::runtime_error("Module not initialized");
    }
    
    // Bind the constant
    module_->attr(name.c_str()) = value;
    
    // Store constant information
    bound_constants_[name] = value;
    
    return *this;
}

// CppClassBinder template method implementation
template<typename Class>
CppClassBinder<Class>::CppClassBinder(pybind11::class_<Class>& py_class) 
    : py_class_(py_class) {
}

template<typename Class>
template<typename... Args>
CppClassBinder<Class>& CppClassBinder<Class>::constructor() {
    py_class_.def(pybind11::init<Args...>());
    return *this;
}

template<typename Class>
template<typename Func>
CppClassBinder<Class>& CppClassBinder<Class>::method(const std::string& name, Func&& func, 
                                                     const std::string& docstring) {
    py_class_.def(name.c_str(), std::forward<Func>(func), docstring.c_str());
    return *this;
}

template<typename Class>
template<typename Func>
CppClassBinder<Class>& CppClassBinder<Class>::static_method(const std::string& name, Func&& func,
                                                            const std::string& docstring) {
    py_class_.def_static(name.c_str(), std::forward<Func>(func), docstring.c_str());
    return *this;
}

template<typename Class>
template<typename T>
CppClassBinder<Class>& CppClassBinder<Class>::property(const std::string& name, T Class::* member) {
    py_class_.def_readwrite(name.c_str(), member);
    return *this;
}

template<typename Class>
template<typename Getter>
CppClassBinder<Class>& CppClassBinder<Class>::property_readonly(const std::string& name, Getter&& getter) {
    py_class_.def_property_readonly(name.c_str(), std::forward<Getter>(getter));
    return *this;
}

template<typename Class>
template<typename Getter, typename Setter>
CppClassBinder<Class>& CppClassBinder<Class>::property(const std::string& name, 
                                                       Getter&& getter, Setter&& setter) {
    py_class_.def_property(name.c_str(), std::forward<Getter>(getter), std::forward<Setter>(setter));
    return *this;
}

// CppFunctionBinder template method implementation
template<typename Func>
CppFunctionBinder::CppFunctionBinder(const std::string& name, Func&& func) 
    : name_(name), function_(std::forward<Func>(func)) {
}

template<typename... Args>
CppFunctionBinder& CppFunctionBinder::default_args(Args&&... args) {
    // Logic for handling default arguments can be implemented here.
    // pybind11 supports setting default values via pybind11::arg().
    return *this;
}

} // namespace cpppy_bridge