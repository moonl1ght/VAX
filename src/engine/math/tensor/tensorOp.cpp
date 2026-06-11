#include "tensorOp.h"
#include <iostream>
#include <iomanip>

using namespace vax::math;

void TensorOp::print(const Tensor &tensor, bool inline_mode) {
    if (inline_mode) {
        for (int i = 0; i < tensor.totalSize(); i++) {
            std::cout << " " << std::fixed << std::setprecision(4) << tensor.data()[i] << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "[";
        for (int i = 0; i < tensor.totalSize(); i++) {
            int index = 1;
            for (std::vector<int>::size_type j = 1; j < tensor.shape().size(); j++) {
                index *= tensor.shape()[tensor.shape().size() - j];
                if (i % index == 0) {
                    std::cout << "[";
                }
            }
            std::cout << " " << std::fixed << std::setprecision(4) << tensor.data()[i] << " ";
            index = 1;
            bool was_closed = false;
            for (std::vector<int>::size_type j = 1; j < tensor.shape().size(); j++) {
                index *= tensor.shape()[tensor.shape().size() - j];
                if ((i + 1) % index == 0) {
                    std::cout << "]";
                    was_closed = true;
                }
            }
            if (was_closed && i != tensor.totalSize() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
}