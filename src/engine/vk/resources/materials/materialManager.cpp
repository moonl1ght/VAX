#include "materialManager.h"
#include <numeric>

using namespace vax;

void MaterialManager::cleanup() {
    _materials.clear();
}

bool MaterialManager::setup() {
    VkDeviceSize bufferSize = sizeof(PBRMaterial) * vax::MAX_MATERIALS;
    auto allocation = vk::Buffer::allocate(
        _device.get(),
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    if (!allocation.has_value()) return false;
    _buffer = std::make_unique<vk::Buffer>(std::move(*allocation));
    _buffer->map();
    _materials.reserve(vax::MAX_MATERIALS);
    _materialsToDelete.reserve(vax::MAX_MATERIALS);
    return true;
}

bool MaterialManager::_updateBuffer(MaterialId id, PBRMaterial material) {
    if (!_buffer) return false;
    auto mappedMemory = _buffer->mappedMemory();
    if (!mappedMemory.has_value()) return false;
    PBRMaterial* materialPtr = static_cast<PBRMaterial*>(*mappedMemory);
    materialPtr[id] = material;
    return true;
}

std::vector<MaterialId> MaterialManager::insertMaterials(std::vector<PBRMaterial> materials) {
    if (_materials.size() + materials.size() >= vax::MAX_MATERIALS) return {};
    if (materials.empty()) return {};
    if (!_buffer) return {};
    auto mappedMemory = _buffer->mappedMemory();
    if (!mappedMemory.has_value()) return {};
    PBRMaterial* materialPtr = static_cast<PBRMaterial*>(*mappedMemory);
    memcpy(materialPtr + _materials.size(), materials.data(), materials.size() * sizeof(PBRMaterial));
    std::vector<MaterialId> ids(materials.size());
    std::iota(ids.begin(), ids.end(), _materials.size() - 1);
    _materials.insert(_materials.end(), materials.begin(), materials.end());
    return ids;
}

MaterialId MaterialManager::insert(PBRMaterial material) {
    if (!_materialsToDelete.empty()) {
        auto id = _materialsToDelete.back();
        if (!_updateBuffer(id, material)) return vax::NullMaterialId;
        _materials[id] = material;
        _materialsToDelete.pop_back();
        return id;
    }
    if (_materials.size() >= vax::MAX_MATERIALS) return vax::NullMaterialId;
    if (!_updateBuffer(_materials.size(), material)) return vax::NullMaterialId;
    _materials.push_back(material);
    return _materials.size() - 1;
}

std::optional<PBRMaterial> MaterialManager::find(MaterialId id) {
    if (id >= _materials.size()) return std::nullopt;
    return _materials[id];
}

void MaterialManager::deleteMaterial(MaterialId id) {
    if (id >= _materials.size()) return;
    _materialsToDelete.push_back(id);
    return;
}