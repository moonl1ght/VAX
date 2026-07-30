#pragma once

#include <glm/glm.hpp>

namespace vax::physics {

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float invMass; // inverse mass, 1/mass
};

} // namespace vax::physics