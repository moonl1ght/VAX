#pragma once

#include "transform.h"

namespace vax::objects {
struct ModelDescriptor {
    enum class ModelExtension {
        GLB, URDF, UNKNOWN
    };

    std::string path;
    vax::math::Transform initialTransform;
    uint32_t instancesCount = 1;

    ModelExtension getModelExtension() const;
    const std::string_view getMainPath() const;
};
} // namespace vax::objects