#pragma once

#include "luna.h"

namespace vax::math {
struct Transform final {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform()
        : position(glm::vec3(0.0f, 0.0f, 0.0f))
        , rotation(glm::vec3(0.0f, 0.0f, 0.0f))
        , scale(glm::vec3(1.0f, 1.0f, 1.0f)) {};
};

struct TransformHandle final {
    glm::mat4 modelMatrix;
    glm::mat3x4 normalMatrix;
    Transform transform;

    TransformHandle()
        : modelMatrix(glm::mat4(1.0f))
        , normalMatrix(glm::mat3x4(1.0f))
        , transform(Transform()) {
        recalculateMatrices();
    };

    void recalculateMatrices();

    void setPosition(const glm::vec3& position) {
        transform.position = position;
        recalculateMatrices();
    }
    void setRotation(const glm::vec3& rotation) {
        transform.rotation = rotation;
        recalculateMatrices();
    }
    void setScale(const glm::vec3& scale) {
        transform.scale = scale;
        recalculateMatrices();
    }

    void setTransform(const Transform& transform) {
        this->transform = transform;
        recalculateMatrices();
    }

    void setModelMatrix(const glm::mat4& modelMatrix) {
        this->modelMatrix = modelMatrix;
        glm::mat3 upperLeft = glm::mat3(modelMatrix);
        normalMatrix = glm::mat3x4(glm::transpose(glm::inverse(upperLeft)));
    }

    template <typename T> void updateTransform(const T& update) {
        update(transform);
        recalculateMatrices();
    }

    glm::mat4 getModelMatrix() const { return modelMatrix; }
    glm::mat3x4 getNormalMatrix() const { return normalMatrix; }
};
} // namespace vax::math