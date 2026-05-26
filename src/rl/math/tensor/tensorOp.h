#pragma once

#include "tensor.h"

namespace vax::rl::math {
class TensorOp final {
  public:
    friend class Tensor;

    static void print(const Tensor& tensor, bool inline_mode = false);
};
} // namespace vax::rl::math