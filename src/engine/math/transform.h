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
            , scale(glm::vec3(1.0f, 1.0f, 1.0f))
        {
        };

        // TODO: presave model matrix and normal matrix, maybe in model istself not in transform
        // when implementing ECS to keep Transform small
        glm::mat4 getModelMatrix() const;
        glm::mat3x4 getNormalMatrix() const;
    };
}