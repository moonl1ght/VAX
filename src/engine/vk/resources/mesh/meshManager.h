#pragma once

#include "luna.h"
#include "device.h"
#include "mesh.h"
#include "resourceHandle.h"

namespace vax {
    class MeshManager final {
    public:
        using MeshResource = std::pair<vax::MeshHandle, vax::objects::Mesh*>;

        explicit MeshManager(const vax::vk::Device& device) : _device(device) {};

        ~MeshManager() {
            fullCleanup();
        }

        MeshManager(const MeshManager& other) = delete;
        MeshManager(MeshManager&& other) noexcept = delete;
        MeshManager& operator=(const MeshManager& other) = delete;
        MeshManager& operator=(MeshManager&& other) noexcept = delete;

        void fullCleanup();

        std::optional<MeshResource> createEmptyMesh();

        std::optional<MeshResource> find(vax::MeshHandle handle);

        bool deleteMesh(vax::MeshHandle handle);

        std::optional<vax::objects::Mesh> detach(vax::MeshHandle handle);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("BufferManager");

        std::reference_wrapper<const vax::vk::Device> _device;
        // TODO: change to vector + use generation for stability
        // maybe vector of vectors of buffers?
        std::unordered_map<MeshId, vax::objects::Mesh> _pool;
        MeshId _lastId = 1;
    };
}