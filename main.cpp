#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #include <glm/vec4.hpp>
// #include <glm/mat4x4.hpp>

#include <iostream>
#include <vulkan/vulkan.h>
#include <nlohmann/json.hpp>

#include "src/apps/SimpleApp.hpp"

int main() {
    std::cout << "run main" << std::endl;
    SimpleApp app = SimpleApp();
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}