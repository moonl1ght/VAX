#pragma once

#include "luna.h"
#include "mesh.h"
#include "resourceHandle.h"
#include "submesh.h"
#include "transform.h"

namespace vax::objects {
class PrimitivesBuilder;
class ModelLoader;
} // namespace vax::objects

namespace vax::objects {
class DrawableModel final {
  public:
    struct Settings {
        bool useWireframe = false;
        bool hasTangents = false;
        bool skipPushConstants = false;
        bool precomputedMVP = false;
    };

    friend class vax::objects::PrimitivesBuilder;
    friend class vax::objects::ModelLoader;

    // TODO: probably remove this one, as it used for standalone drawing but prefer scene node drawing.
    std::optional<vax::math::TransformHandle> transformHandle;

    vax::math::TransformMatrixHandle transformMatrixHandle;

    struct DrawContext {};

    explicit DrawableModel(vax::MeshManager& meshManager, vax::MeshHandle meshHandle)
        : _meshManager(meshManager)
        , _meshHandle(meshHandle) {};

    DrawableModel(DrawableModel&& other) noexcept = default;
    DrawableModel& operator=(DrawableModel&& other) noexcept = default;

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    ~DrawableModel() {};

    bool loadMesh(vax::vk::CommandBuffer& commandBuffer);

    void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

    Settings& settings() { return _settings; }
    const Settings& settings() const { return _settings; }
    void setSettings(const Settings& settings) { _settings = settings; }

    vax::objects::Submesh& submesh(size_t index) { return _submeshes[index]; }
    const vax::objects::Submesh& submesh(size_t index) const { return _submeshes[index]; }
    size_t submeshCount() const { return _submeshes.size(); }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("DrawableModel");

    std::reference_wrapper<vax::MeshManager> _meshManager;

    vax::MeshHandle _meshHandle;

    // TODO: remove this will need to use mesh manager to get the mesh
    vax::objects::MeshPBR* _mesh;
    std::vector<vax::objects::Submesh> _submeshes;
    Settings _settings;
};
} // namespace vax::objects