#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/vec4.hpp>
// #include <glm/mat4x4.hpp>

#include <iostream>
#include <vulkan/vulkan.h>
#include <nlohmann/json.hpp>

#include "src/app/App.hpp"

#include <thread>
#include <functional>

void doSomething(std::function<void()> callback) {
    callback();
}

class Buf {
    Buf();
    Buf(const Buf& other);
    Buf& operator=(const Buf& other);

    Buf(Buf&& other) noexcept;
    Buf& operator=(Buf&& other) noexcept;

    virtual ~Buf();
};

int i = 0;
std::mutex mutex;

void task1() {
    std::lock_guard<std::mutex> lock(mutex);
    i++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task 1 completed" << i << std::endl;
}

void task2() {
    std::lock_guard<std::mutex> lock(mutex);
    i++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task 2 completed" << i << std::endl;
}

template<typename Iterator1, typename Iterator2, typename Condition, typename Transformer>
void trasnfrom_if(Iterator1 begin, Iterator1 end, Iterator2 result, Condition condition, Transformer transformer) {
    for (; begin != end; begin++) {
        std::cout << *begin << std::endl;
        if (condition(*begin)) {
            *result = transformer(*begin);
        } else {
            *result = *begin;
        }
        result++;
    }
}

template<template<typename...> class Container, typename T, template<typename...> class Container2, typename T2, typename Condition, typename Transformer>
void trasnfrom_if1(const Container<T>& v1, Container2<T2>& v2, Condition condition, Transformer transformer) {
    for (const auto& value: v1) {
        if (condition(value)) {
            v2.push_back(transformer(value));
        } else {
            v2.push_back(value);
        }
    }
}

template <template <typename...> class Container, typename T>
void processContainer(const Container<T>& container) {
    std::cout << "Processing container with " << container.size() << " elements:\n";
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << "\n";
}

void main2() {
    std::vector<int> v1 { 1, 2, 3, 4, 5, 6};
    std::vector<int> v2;
    v2.resize(v1.size());

    doSomething([]() { std::cout << "callback" << std::endl; });

    // trasnfrom_if(v1.begin(), v1.end(), v2.begin(), [](int a) { return a % 2 == 0; }, [](int a) { return a*a; });

    trasnfrom_if1(v1, v2, [](int a) { return a % 2 == 0; }, [](int a) { return a*a; });


    std::for_each(v2.begin(), v2.end(), [](int n) {
        static bool first = true;
        if (!first) {
            std::cout << " , ";
        }
        std::cout << n;
        first = false;
    });
    std::cout << '\n';
}

int main() {
    Logger::getInstance().log("Run application");
    App app = App();
    try {
        app.run();
    } catch (const std::exception& e) {
        Logger::getInstance().error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}