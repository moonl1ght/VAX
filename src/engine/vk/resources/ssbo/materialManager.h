#pragma once

#include "buffer.h"
#include "device.h"
#include "luna.h"
#include "resourceUtils.h"
#include "shaderUniforms.h"

namespace vax {
class MaterialManager final {
  public:
    explicit MaterialManager(const vax::vk::Device& device)
        : _device(device) {};

    ~MaterialManager() { cleanup(); }

    MaterialManager(const MaterialManager& other) = delete;
    MaterialManager(MaterialManager&& other) noexcept = delete;
    MaterialManager& operator=(const MaterialManager& other) = delete;
    MaterialManager& operator=(MaterialManager&& other) noexcept = delete;

    bool setup();

    void cleanup();

    MaterialId insert(PBRMaterial material);

    std::vector<MaterialId> insertMaterials(std::vector<PBRMaterial> materials);

    std::optional<PBRMaterial> find(MaterialId id);

    void deleteMaterial(MaterialId id);

    const vax::vk::Buffer& materialBuffer() const { return *_buffer; }

  private:
    vax::Logger _logger = vax::Logger("MaterialManager");
    std::reference_wrapper<const vax::vk::Device> _device;
    // TODO: change to vector of buffers to handle material overflow
    std::unique_ptr<vax::vk::Buffer> _buffer = nullptr;
    std::vector<PBRMaterial> _materials;
    std::vector<MaterialId> _materialsToDelete;

    bool _updateBuffer(MaterialId id, PBRMaterial material);
};
} // namespace vax