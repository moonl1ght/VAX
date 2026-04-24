#pragma once

#include "drawableModel.h"

namespace vax::objects::Primitives2D {

    inline vax::objects::DrawableModel* createTriangle(const vax::vk::Device& device) {
        std::unique_ptr<vax::objects::Mesh> mesh = std::make_unique<vax::objects::Mesh>(device);
        mesh->setVertices({
            {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}
        });
        return new vax::objects::DrawableModel(std::move(mesh));
    }

    inline vax::objects::DrawableModel* createPlane(const vax::vk::Device& device) {
        std::unique_ptr<vax::objects::Mesh> mesh = std::make_unique<vax::objects::Mesh>(device);
        mesh->setVertices({
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        });
        mesh->setIndices({ 0, 1, 2, 2, 3, 0 });
        return new vax::objects::DrawableModel(std::move(mesh));
    }

}