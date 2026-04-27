#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "meshManager.h"

namespace vax::objects {
    class PrimitivesBuilder {
    public:
        explicit PrimitivesBuilder(vax::MeshManager& meshManager) : _meshManager(meshManager) {};

        ~PrimitivesBuilder() {};

        PrimitivesBuilder(const PrimitivesBuilder& other) = delete;
        PrimitivesBuilder(PrimitivesBuilder&& other) noexcept = delete;
        PrimitivesBuilder& operator=(const PrimitivesBuilder& other) = delete;
        PrimitivesBuilder& operator=(PrimitivesBuilder&& other) noexcept = delete;

        std::optional<vax::objects::DrawableModel> createCube();

    private:
        std::reference_wrapper<vax::MeshManager> _meshManager;
    };
}