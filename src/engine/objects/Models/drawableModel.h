#pragma once

#include "drawContext.h"
#include "luna.h"
#include "mesh.h"
#include "resourceHandle.h"
#include "ssboManager.h"
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
        bool instanceDrawing = false;
    };

    friend class vax::objects::PrimitivesBuilder;
    friend class vax::objects::ModelLoader;

    explicit DrawableModel(vax::MeshManager& meshManager, vax::SSBOManager& ssboManager, vax::MeshHandle meshHandle)
        : _meshManager(meshManager)
        , _ssboManager(ssboManager)
        , _meshHandle(meshHandle) {};

    DrawableModel(DrawableModel&& other) noexcept = default;
    DrawableModel& operator=(DrawableModel&& other) noexcept = default;

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    ~DrawableModel() {};

    bool loadMesh(const vax::objects::MeshPBR::LoadMeshBuffersContext& context);

    void
    updateSSBO(uint32_t currentFrame, std::vector<vax::math::TransformMatrixHandle> instanceTransformMatrixHandles);

    void draw(const vax::renderer::DrawContext& drawContext);

    Settings& settings() { return _settings; }

    const Settings& settings() const { return _settings; }

    void setSettings(const Settings& settings) { _settings = settings; }

    vax::objects::Submesh& submesh(size_t index) { return _submeshes[index]; }

    const vax::objects::Submesh& submesh(size_t index) const { return _submeshes[index]; }

    size_t submeshCount() const { return _submeshes.size(); }

    void updateSSBOHandle(vax::SSBOManager::SSBOHandle ssboHandle) { _ssboHandle = ssboHandle; }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("DrawableModel");

    std::reference_wrapper<vax::MeshManager> _meshManager;
    std::reference_wrapper<vax::SSBOManager> _ssboManager;

    vax::MeshHandle _meshHandle;

    // TODO: remove this will need to use mesh manager to get the mesh
    vax::objects::MeshPBR* _mesh;
    std::vector<vax::objects::Submesh> _submeshes;
    Settings _settings;
    uint32_t _instancesCount = 1;
    uint32_t _ssboHandle = vax::SSBOManager::NullSSBOHandle;
};
} // namespace vax::objects