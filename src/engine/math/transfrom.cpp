#include "transform.h"

using namespace vax::math;

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