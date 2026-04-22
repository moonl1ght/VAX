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
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED FATAL_ERROR)
find_package(vk-bootstrap REQUIRED FATAL_ERROR)
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64/lib/cmake/SDL3")
    find_package(SDL3 REQUIRED)
else()
    find_package(SDL3 REQUIRED FATAL_ERROR)
    find_package(imgui REQUIRED)
    find_package(tinyobjloader REQUIRED FATAL_ERROR)
endif()
# find_package(imgui REQUIRED CONFIG)

# find_package(glm REQUIRED FATAL_ERROR)
if(DEFINED ENV{VULKAN_SDK})
    message(STATUS "VULKAN_SDK is set to: $ENV{VULKAN_SDK}")
else()
    message(WARNING "VULKAN_SDK environment variable is not set.")
endif()

add_compile_definitions(PROJ_DIR="${PROJECT_SOURCE_DIR}")

if(WIN32)
    file(GLOB _SLANG_BIN_HINTS "C:/slang-*/bin")
elseif(APPLE)
    file(GLOB _SLANG_BIN_HINTS "/Applications/Slang.app/Contents/MacOS/bin")
endif()

if(APPLE)
    set(SLANG_DIR "/Users/alexander/slang-2026/bin/")
endif()

find_program(SLANG_COMPILER slangc HINTS
    $ENV{SLANG_DIR}/bin
    ${_SLANG_BIN_HINTS}
    $ENV{VULKAN_SDK}/Bin
    $ENV{VULKAN_SDK}/Bin32
    /usr/bin
    /usr/local/bin
)
if(NOT SLANG_COMPILER)
    message(FATAL_ERROR "slangc not found. Install Slang and ensure it is on PATH or set SLANG_DIR.")
endif()
message(STATUS "Found slangc: ${SLANG_COMPILER}")

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

set(STAGE_NAME_vert vertex)
set(STAGE_NAME_frag fragment)
set(STAGE_NAME_comp compute)
set(STAGE_NAME_geom geometry)
set(STAGE_NAME_tesc hull)
set(STAGE_NAME_tese domain)

set(ENTRY_NAME_vert vertMain)
set(ENTRY_NAME_frag fragMain)
set(ENTRY_NAME_comp compMain)
set(ENTRY_NAME_geom geomMain)
set(ENTRY_NAME_tesc tescMain)
set(ENTRY_NAME_tese teseMain)

file(GLOB_RECURSE SLANG_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.vert.slang"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.frag.slang"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.comp.slang"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.geom.slang"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.tesc.slang"
    "${PROJECT_SOURCE_DIR}/src/engine/shaders/*.tese.slang"
)

file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/engine/shaders/out")

foreach(SLANG ${SLANG_SOURCE_FILES})
    string(REGEX MATCH "([^/\\]+)\\.([^.]+)\\.slang$" _ "${SLANG}")
    set(BASE_NAME    "${CMAKE_MATCH_1}")
    set(SUFFIX       "${CMAKE_MATCH_2}")
    set(STAGE        "${STAGE_NAME_${SUFFIX}}")
    set(EP           "${ENTRY_NAME_${SUFFIX}}")
    set(SPIRV        "${PROJECT_SOURCE_DIR}/src/engine/shaders/out/${BASE_NAME}.${SUFFIX}.spv")
    message(STATUS "BUILDING SLANG SHADER: ${BASE_NAME} [${STAGE}]")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/src/engine/shaders/out"
        COMMAND ${SLANG_COMPILER} "${SLANG}"
            -entry ${EP}
            -stage ${STAGE}
            -target spirv
            -o "${SPIRV}"
        DEPENDS ${SLANG}
        COMMENT "Compiling ${BASE_NAME} [${STAGE}]"
        VERBATIM
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

macro(HEADER_DIRECTORIES return_list)
    file(GLOB_RECURSE new_list "${PROJECT_SOURCE_DIR}/src/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.h")
    set(dir_list "")

    foreach(file_path ${new_list})
        get_filename_component(dir_path ${file_path} PATH)
        set(dir_list ${dir_list} ${dir_path})
    endforeach()

    list(REMOVE_DUPLICATES dir_list)
    set(${return_list} ${dir_list})
endmacro()

HEADER_DIRECTORIES(header_list)
add_executable(Luna main.cpp ${all_SRCS})
target_include_directories(Luna PUBLIC ${header_list})
add_dependencies(Luna Shaders)

if(WIN32)
    add_custom_command(TARGET Luna POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/SDL3.dll"
        $<TARGET_FILE_DIR:Luna>/SDL3.dll)
endif()

# target_link_libraries(Luna)
# add_executable(Luna main.cpp ${PROJECT_SOURCE_DIR}/src/apps/App.hpp ${PROJECT_SOURCE_DIR}/src/apps/App.cpp ${PROJECT_SOURCE_DIR}/src/apps/SimpleApp.hpp ${PROJECT_SOURCE_DIR}/src/apps/SimpleApp.cpp)

# target_include_directories(your_executable_name PRIVATE ${PROJECT_SOURCE_DIR}/include ${CONAN_INCLUDE_DIRS})
# find_package(nlohmann REQUIRED FATAL_ERROR)
if(WIN32)
    if(VULKAN_FOUND)
        message(STATUS "Found Vulkan. Including and linking...")
        target_link_libraries(
            ${PROJECT_NAME}
            Vulkan::Vulkan
            nlohmann_json::nlohmann_json
            assimp::assimp
            vk-bootstrap::vk-bootstrap
            SDL3::SDL3
            glm::glm
        )
    endif(VULKAN_FOUND)
else()
    if(VULKAN_FOUND)
        message(STATUS "Found Vulkan. Including and linking...")
        target_link_libraries(
            ${PROJECT_NAME}
            Vulkan::Vulkan
            nlohmann_json::nlohmann_json
            assimp::assimp
            vk-bootstrap::vk-bootstrap
            tinyobjloader::tinyobjloader
            SDL3::SDL3
            glm::glm
            imgui::imgui
        )
    endif(VULKAN_FOUND)
endif()
