cmake_minimum_required(VERSION 3.10.0)
project(Luna VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_LIST_DIR}/build/)
# target_include_directories(Luna PRIVATE ${PROJECT_SOURCE_DIR}/include ${CONAN_INCLUDE_DIRS})
find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(glfw3 REQUIRED FATAL_ERROR)
# set(CMAKE_PREFIX_PATH="build/assimp-config.cmake")
# set(-DCMAKE_TOOLCHAIN_FILE="build/assimp-config.cmake")
find_package(assimp REQUIRED FATAL_ERROR)
find_package(nlohmann_json REQUIRED FATAL_ERROR)
# find_package(glm REQUIRED FATAL_ERROR)

add_executable(Luna main.cpp)


# target_include_directories(your_executable_name PRIVATE ${PROJECT_SOURCE_DIR}/include ${CONAN_INCLUDE_DIRS})
# find_package(nlohmann REQUIRED FATAL_ERROR)

if (VULKAN_FOUND)
    message(STATUS "Found Vulkan. Including and linking...")
    target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw nlohmann_json::nlohmann_json assimp::assimp)
endif(VULKAN_FOUND)