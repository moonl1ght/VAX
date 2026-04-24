#include "meshManager.h"

using namespace vax;

void vax::MeshManager::fullCleanup() {
    for (auto& [handle, mesh] : _pool) {
        mesh._destroy();
    }
    _pool.clear();
}

std::optional<MeshManager::MeshResource> MeshManager::createEmptyMesh() {
    auto mesh = vax::objects::Mesh(_device.get());
    mesh._isDetached = false;
    mesh._id = _lastId++;
    auto [it, inserted] = _pool.try_emplace(mesh.id(), std::move(mesh));
    if (!inserted) {
        mesh._destroy();
        return std::nullopt;
    }
    return std::make_pair(it->first, &it->second);
}

std::optional<MeshManager::MeshResource> MeshManager::find(vax::MeshHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return std::nullopt;
    return std::make_pair(handle, &it->second);
}

bool MeshManager::deleteMesh(vax::MeshHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return false;
    it->second._destroy();
    _pool.erase(it);
    return true;
}

std::optional<vax::objects::Mesh> MeshManager::detach(vax::MeshHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end()) return std::nullopt;
    it->second._detach();
    return std::move(it->second);
}