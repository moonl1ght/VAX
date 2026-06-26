#pragma once

#include "colorPalette.h"
#include "transform.h"

namespace vax::objects {
struct ModelDescriptor {
    enum class ModelExtension { GLB, URDF, UNKNOWN };

    enum class ModelType { MODEL, PRIMITIVE_CUBE, PRIMITIVE_PLANE };

    struct PrimitiveDescriptor {
        float size = 1.0f;
        vax::Color color = vax::ColorPalette::White;
    };

    std::string path;
    std::string name;
    ModelType modelType;
    vax::math::Transform initialTransform;
    PrimitiveDescriptor primitiveDescriptor;
    uint32_t instancesCount = 1;

    ModelExtension getModelExtension() const;
    const std::string_view getMainPath() const;
};
} // namespace vax::objects