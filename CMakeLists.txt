cmake_minimum_required(VERSION 3.10.0)

# set( CMAKE_CXX_COMPILER "C:\\msys64\\ucrt64\\bin\\g++" )
# set( CMAKE_C_COMPILER "C:\\msys64\\ucrt64\\bin\\gcc" )
# set( CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++" )
# set( CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc" )
project(Luna VERSION 0.1.0 LANGUAGES C CXX)

# set( CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++" )
# set( CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc" )
# set(CMAKE_GENERATOR MinGW Makefiles)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_CURRENT_LIST_DIR}/build/)

# target_include_directories(Luna PRIVATE ${PROJECT_SOURCE_DIR}/include ${CONAN_INCLUDE_DIRS})
find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(glfw3 REQUIRED FATAL_ERROR)

# set(CMAKE_PREFIX_PATH="build/assimp-config.cmake")
# set(-DCMAKE_TOOLCHAIN_FILE="build/assimp-config.cmake")
find_package(assimp REQUIRED FATAL_ERROR)
find_package(nlohmann_json REQUIRED FATAL_ERROR)
find_package(vk-bootstrap REQUIRED FATAL_ERROR)
find_package(tinyobjloader REQUIRED FATAL_ERROR)
find_package(SDL2 REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)

# find_package(glm REQUIRED FATAL_ERROR)
if(DEFINED ENV{VULKAN_SDK})
    message(STATUS "VULKAN_SDK is set to: $ENV{VULKAN_SDK}")
else()
    message(WARNING "VULKAN_SDK environment variable is not set.")
endif()

add_compile_definitions(PROJ_DIR="${PROJECT_SOURCE_DIR}")

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

# add_subdirectory(src/)
# target_sources(src/**.cpp)
include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE all_SRCS
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.spv"
)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.comp"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "BUILDING SHADER")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/src/engine/shaders/out/${FILE_NAME}.spv")
    message(STATUS ${GLSL})
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list "${PROJECT_SOURCE_DIR}/src/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.h")
    SET(dir_list "")

    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()

    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(header_list)
add_executable(Luna main.cpp ${all_SRCS})
target_include_directories(Luna PUBLIC ${header_list})

# target_link_libraries(Luna)
# add_executable(Luna main.cpp ${PROJECT_SOURCE_DIR}/src/apps/App.hpp ${PROJECT_SOURCE_DIR}/src/apps/App.cpp ${PROJECT_SOURCE_DIR}/src/apps/SimpleApp.hpp ${PROJECT_SOURCE_DIR}/src/apps/SimpleApp.cpp)

# target_include_directories(your_executable_name PRIVATE ${PROJECT_SOURCE_DIR}/include ${CONAN_INCLUDE_DIRS})
# find_package(nlohmann REQUIRED FATAL_ERROR)
if(VULKAN_FOUND)
    message(STATUS "Found Vulkan. Including and linking...")
    target_link_libraries(
        ${PROJECT_NAME}
        Vulkan::Vulkan
        glfw
        nlohmann_json::nlohmann_json
        assimp::assimp
        vk-bootstrap::vk-bootstrap
        tinyobjloader::tinyobjloader
        SDL2::SDL2
        imgui::imgui
    )
endif(VULKAN_FOUND)