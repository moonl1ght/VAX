#pragma once

#include "colorPalette.h"
#include "transform.h"

namespace vax::engine {
struct ModelDescriptor {
    enum class ModelExtension { GLB, URDF, UNKNOWN };

    enum class ModelType { MODEL, PRIMITIVE_CUBE, PRIMITIVE_PLANE };

    struct PrimitiveDescriptor {
        float size = 1.0f;
        Color color = ColorPalette::White;
    };

    std::string path;
    std::string id;
    ModelType modelType;
    std::vector<vax::math::Transform> transforms;
    PrimitiveDescriptor primitiveDescriptor;
    uint32_t instancesCount = 1;
    bool isIdentifiable = true;
    ModelExtension getModelExtension() const;
    const std::string_view getMainPath() const;
};
} // namespace vax::engine