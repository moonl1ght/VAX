#ifndef Transform_hpp
#define Transform_hpp

#include "luna.h"

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform()
    : position(glm::vec3(0.0f, 0.0f, 0.0f))
    , rotation(glm::vec3(0.0f, 0.0f, 0.0f))
    , scale(glm::vec3(1.0f, 1.0f, 1.0f))
    { };

    glm::mat4 getModelMatrix() const;
};

#endif