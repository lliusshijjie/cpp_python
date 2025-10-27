#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

namespace example_lib {

class MathCalculator {
public:
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);
};

class StringProcessor {
public:
    std::string concatenate(const std::string& a, const std::string& b);
    int get_length(const std::string& s);
};

class DataContainer {
public:
    void add_data(const std::string& key, int value);
    int get_data(const std::string& key);
    std::map<std::string, int> get_all_data();
private:
    std::map<std::string, int> data_;
};

class CallbackExample {
public:
    using CallbackType = std::function<void(const std::string&)>;
    void execute_callback(CallbackType callback);
};

std::string global_function_example(const std::string& input);

}