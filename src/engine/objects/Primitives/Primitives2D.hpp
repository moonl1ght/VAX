#ifndef Primitives2D_hpp
#define Primitives2D_hpp

#include "Mesh.hpp"
#include <iostream>

namespace Primitives2D {

inline Mesh *createTriangle() {
    Mesh *mesh = new Mesh();
    mesh->vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
    return mesh;
}

inline Mesh *createPlane() {
    Mesh *mesh = new Mesh();
    mesh->vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
    mesh->indices = {0, 1, 2, 2, 3, 0};
    return mesh;
}

} // namespace Primitives2D

#endif