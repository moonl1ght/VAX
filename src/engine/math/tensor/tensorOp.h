#pragma once

#include "tensor.h"

namespace vax::math {
class TensorOp final {
  public:
    static void print(const Tensor& tensor, bool inline_mode = false);

    static Tensor argmax(const Tensor& tensor, int axis = -1);
};

class TensorOpGpu final {
};
} // namespace vax::math