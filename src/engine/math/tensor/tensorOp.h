#pragma once

#include "tensor.h"

namespace vax::math {
class TensorOp final {
  public:
    friend class Tensor;

    static void print(const Tensor& tensor, bool inline_mode = false);
};
} // namespace vax::math