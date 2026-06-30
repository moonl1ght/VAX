#pragma once

#include <vector>

namespace vax::rl::math {
template <typename T> struct Position2D final {
    T x;
    T y;
};

using Position2DInt = Position2D<int>;
using Position2DFloat = Position2D<float>;
using Position2DDouble = Position2D<double>;

bool isBorderIndex(std::vector<int> indices, std::vector<int> dimensions);
} // namespace vax::rl::math