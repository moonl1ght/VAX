#pragma once

#include "luna.h"
#include "model.h"
#include "mesh.h"
#include "pipelineManager.h"
#include "shaderUniforms.h"
#include "resourceHandle.h"
#include "vkEngine.h"
#include "submesh.h"

namespace vax::objects {
    class PrimitivesBuilder;
    class ModelLoader;
}

namespace vax::objects {
    class DrawableModel : public Model {
    public:
        friend class vax::objects::PrimitivesBuilder;
        friend class vax::objects::ModelLoader;

        struct DrawContext {

        };

        explicit DrawableModel(
            vax::MeshManager& meshManager,
            vax::MeshHandle meshHandle
        )
            : _meshManager(meshManager)
            , _meshHandle(meshHandle) {
        };

        ~DrawableModel() {};

        void draw(
            vax::vk::Engine* vkEngine,
            VkCommandBuffer commandBuffer,
            const vax::vk::PipelineManager& pipelineManager,
            float time
        );

    private:
        vax::utils::Logger _logger = vax::utils::Logger("DrawableModel");

        std::reference_wrapper<vax::MeshManager> _meshManager;

        vax::MeshHandle _meshHandle;

        // TODO: remove this will need to use mesh manager to get the mesh
        vax::objects::Mesh* _mesh;
    };
}