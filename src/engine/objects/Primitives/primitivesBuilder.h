#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "meshManager.h"

namespace vax::objects {
    class PrimitivesBuilder {
    public:
        explicit PrimitivesBuilder(vax::MeshManager& meshManager) : _meshManager(meshManager) {};

        ~PrimitivesBuilder() {};

        std::optional<vax::objects::DrawableModel> createCube();

    private:
        std::reference_wrapper<vax::MeshManager> _meshManager;
    };
}