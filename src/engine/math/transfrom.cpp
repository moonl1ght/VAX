#include "transform.h"

using namespace vax::math;

glm::mat4 Transform::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

glm::mat3x4 Transform::getNormalMatrix() const {
    glm::mat4 model = getModelMatrix();
    glm::mat3 upperLeft = glm::mat3(model);
    // TODO: check if model is uniform scale or not to avoid unnecessary inverse and transpose
    // return glm::mat3x4(glm::transpose(glm::inverse(upperLeft)));
    return glm::mat3x4(upperLeft);
}