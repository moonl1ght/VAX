#pragma once
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VK_PROTOTYPES
#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <limits>
#include <optional>
#include <set>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include <vulkan/vulkan_core.h>

#include "logger.h"
#include "tinyobjloader.hpp"
#include "vma.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#define RELATIVE_PATH(val) std::string(PROJ_DIR) + "/" + val
#define SRC_PATH(val) std::string(PROJ_DIR) + "/src/" + val
#define RES_PATH(val) std::string(PROJ_DIR) + "/resources/" + val

#ifdef __APPLE__
#define MACOS true
#else
#define MACOS false
#endif

#define SDL_MAIN_HANDLED
#ifdef __APPLE__
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#else
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#endif