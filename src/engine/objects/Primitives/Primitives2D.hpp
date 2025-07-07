#ifndef Primitives2D_hpp
#define Primitives2D_hpp

#include "DrawableModel.hpp"

namespace Primitives2D {

    inline DrawableModel* createTriangle() {
        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
        mesh->vertices = {
            {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}
        };
        return new DrawableModel(std::move(mesh));
    }

    inline DrawableModel* createPlane() {
        std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
        mesh->vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };
        mesh->indices = { 0, 1, 2, 2, 3, 0 };
        return new DrawableModel(std::move(mesh));
    }

} // namespace Primitives2D

#endif