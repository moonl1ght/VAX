#include "meshManager.h"
#include "luna.h"

using namespace vax::vk;

void MeshManager::loadGlobalBuffers(size_t size) {
    VkDeviceSize targetSizeInBytes = size * 1024ULL * 1024;
    VkDeviceSize vertexStride = sizeof(Vertex);

    size_t maxVertexCount = targetSizeInBytes / vertexStride;
    VkDeviceSize alignedBufferSize = maxVertexCount * vertexStride;
    VkDeviceSize indexBufferSize = maxVertexCount * 3 * sizeof(uint32_t);
    auto buffer = VertexBuffer::allocate(
        _device.get(),
        "global_vertex_buffer",
        alignedBufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
    );
    if (!buffer) {
        _logger.error("Failed to allocate global vertex buffer");
        return;
    }
    _globalVertexBuffers.push_back(std::make_unique<VertexBuffer>(std::move(*buffer)));
    auto indexBuffer = IndexBuffer::allocate(
        _device.get(),
        "global_index_buffer",
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT
    );
    if (!indexBuffer) {
        _logger.error("Failed to allocate global index buffer");
        return;
    }
    _globalIndexBuffers.push_back(std::make_unique<IndexBuffer>(std::move(*indexBuffer)));
}

void MeshManager::fullCleanup() {
    for (auto& [handle, mesh] : _pool) {
        mesh._destroy();
    }
    _pool.clear();
}

std::optional<MeshManager::MeshResource> MeshManager::createEmptyMesh() {
    auto mesh = Mesh(_device.get(), *this);
    mesh._id = _lastId++;
    auto [it, inserted] = _pool.try_emplace(mesh.id(), std::move(mesh));
    if (!inserted) {
        return std::nullopt;
    }
    return std::make_pair(it->first, &it->second);
}

std::optional<MeshManager::MeshResource> MeshManager::find(MeshHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end())
        return std::nullopt;
    return std::make_pair(handle, &it->second);
}

bool MeshManager::deleteMesh(MeshHandle handle) {
    auto it = _pool.find(handle.id());
    if (it == _pool.end())
        return false;
    it->second._destroy();
    _pool.erase(it);
    return true;
}