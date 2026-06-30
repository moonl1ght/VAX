#pragma once

#include "drawContext.h"
#include "luna.h"
#include "mesh.h"
#include "resourceHandle.h"
#include "submesh.h"

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

    explicit DrawableModel(vax::MeshManager& meshManager, vax::MeshHandle meshHandle)
        : _meshManager(meshManager)
        , _meshHandle(meshHandle) {};

    DrawableModel(DrawableModel&& other) noexcept = default;
    DrawableModel& operator=(DrawableModel&& other) noexcept = default;

    DrawableModel(const DrawableModel& other) = delete;
    DrawableModel& operator=(const DrawableModel& other) = delete;

    ~DrawableModel() {};

    bool loadMesh(const vax::objects::MeshPBR::LoadMeshBuffersContext& context);

    void draw(const vax::renderer::DrawContext& drawContext, uint32_t instanceOffset, uint32_t instancesCount);

    Settings& settings() { return _settings; }

    const Settings& settings() const { return _settings; }

    void setSettings(const Settings& settings) { _settings = settings; }

    vax::objects::Submesh& submesh(size_t index) { return _submeshes[index]; }

    const vax::objects::Submesh& submesh(size_t index) const { return _submeshes[index]; }

    size_t submeshCount() const { return _submeshes.size(); }

  private:
    vax::Logger _logger = vax::Logger("DrawableModel");

    std::reference_wrapper<vax::MeshManager> _meshManager;

    vax::MeshHandle _meshHandle;

    // TODO: remove this will need to use mesh manager to get the mesh
    vax::objects::MeshPBR* _mesh;
    std::vector<vax::objects::Submesh> _submeshes;
    Settings _settings;
};

struct DrawableModelHandle final {
    using InstanceDrawingRange = std::pair<uint32_t, uint32_t>;
    DrawableModel* drawableModel;

    std::vector<InstanceDrawingRange> instanceDrawingRanges;
};
} // namespace vax::objects