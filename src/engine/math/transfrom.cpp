#include "transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace vax::math;

Transform::Transform(glm::mat4 modelMatrix) {
    position = glm::vec3(modelMatrix[3]);

    scale.x = glm::length(glm::vec3(modelMatrix[0]));
    scale.y = glm::length(glm::vec3(modelMatrix[1]));
    scale.z = glm::length(glm::vec3(modelMatrix[2]));

    glm::mat3 rotationMatrix(
        glm::vec3(modelMatrix[0]) / scale.x,
        glm::vec3(modelMatrix[1]) / scale.y,
        glm::vec3(modelMatrix[2]) / scale.z
    );

    glm::quat rotationQuat = glm::quat_cast(rotationMatrix);
    rotation = glm::eulerAngles(rotationQuat);
}

void Transform::updateRotationWithQuaternion(const glm::quat& rotation) {
    this->rotation = glm::eulerAngles(rotation);
}

void Transform::updateRotationInDegrees(const glm::vec3& rotation) {
    this->rotation = glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
}

glm::vec3 Transform::getRotationInDegrees() const {
    return glm::vec3(glm::degrees(rotation.x), glm::degrees(rotation.y), glm::degrees(rotation.z));
}

glm::mat4 Transform::getModelMatrix() const {
    glm::quat rotationQuat = glm::quat(rotation);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuat);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void TransformHandle::recalculateMatrices() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);
    modelMatrix = model;
    glm::mat3 upperLeft = glm::mat3(model);
    normalMatrix = glm::mat3x4(glm::transpose(glm::inverse(upperLeft)));
}