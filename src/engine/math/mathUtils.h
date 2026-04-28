#pragma once

#include "luna.h"

namespace vax::math {
    using Degree = float;
    using Radian = float;

    constexpr Radian radians_from_degrees(Degree degrees) {
        return glm::radians(degrees);
    }

    constexpr Degree degrees_from_radians(Radian radians) {
        return glm::degrees(radians);
    }

    constexpr glm::mat4 translationMatrix(const glm::vec3& position) {
        glm::mat4 transform = glm::mat4(1.0f);
        transform[3] = transform[3] + glm::vec4(position, 0.0f);
        return transform;
    }

    constexpr glm::mat4 eulerAngleXYZRotationMatrix(const glm::vec3& rotation) {
        glm::mat4 rotationM = glm::mat4(1.0f);
        rotationM = glm::rotate(rotationM, rotation.z, glm::vec3(0, 0, 1));
        rotationM = glm::rotate(rotationM, rotation.y, glm::vec3(0, 1, 0));
        rotationM = glm::rotate(rotationM, rotation.x, glm::vec3(1, 0, 0));
        return rotationM;
    }
}