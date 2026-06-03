#pragma once

#include <vector>

namespace vax::rl::math::utils {
struct Position2D final {
    int x;
    int y;
};

bool isBorderIndex(std::vector<int> indices, std::vector<int> dimensions);
} // namespace vax::rl::math::utils