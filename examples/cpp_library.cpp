#include "cpp_library.h"
#include <stdexcept>

namespace example_lib {

double MathCalculator::add(double a, double b) { return a + b; }
double MathCalculator::subtract(double a, double b) { return a - b; }
double MathCalculator::multiply(double a, double b) { return a * b; }
double MathCalculator::divide(double a, double b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
}

std::string StringProcessor::concatenate(const std::string& a, const std::string& b) {
    return a + b;
}

int StringProcessor::get_length(const std::string& s) {
    return s.length();
}

void DataContainer::add_data(const std::string& key, int value) {
    data_[key] = value;
}

int DataContainer::get_data(const std::string& key) {
    return data_.at(key);
}

std::map<std::string, int> DataContainer::get_all_data() {
    return data_;
}

void CallbackExample::execute_callback(CallbackType callback) {
    callback("Hello from C++");
}

std::string global_function_example(const std::string& input) {
    return "Processed: " + input;
}

}