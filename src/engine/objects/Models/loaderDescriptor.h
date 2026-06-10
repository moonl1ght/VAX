#pragma once

#include "transform.h"

namespace vax::objects {
struct LoaderDescriptor {
    enum class ModelExtension {
        GLB, URDF, UNKNOWN
    };

    std::string path;
    vax::math::Transform initialTransform;

    ModelExtension getModelExtension() const;
};
} // namespace vax::objects