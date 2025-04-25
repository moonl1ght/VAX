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

#define RELATIVE_PATH(val) std::string(PROJ_DIR) + "/" + val
#define SRC_PATH(val) std::string(PROJ_DIR) + "/src/" + val
