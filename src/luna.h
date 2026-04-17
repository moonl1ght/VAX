#pragma once
#ifdef __APPLE__
#define VK_USE_PLATFORM_METAL_EXT
#else
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#define VK_PROTOTYPES
#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include <set>
#include <optional>
#include <vector>
#include <algorithm>
#include <cstring>
#include <limits>
#include <fstream>
#include <array>
#include <utility>

#include "logger.h"
#include "vma.hpp"
#include "tinyobjloader.hpp"

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