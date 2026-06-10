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
    const std::string_view getMainPath() const;
};
} // namespace vax::objects