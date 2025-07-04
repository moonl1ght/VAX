#ifndef luna_h
#define luna_h

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

#include "Logger.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#define RELATIVE_PATH(val) std::string(PROJ_DIR) + "/" + val
#define SRC_PATH(val) std::string(PROJ_DIR) + "/src/" + val
#define RES_PATH(val) std::string(PROJ_DIR) + "/resources/" + val

#define MACOS true

#endif