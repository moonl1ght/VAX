#pragma once

#include "resourceUtils.h"

namespace vax {
    template<typename T, T NullId>
    class ResourceHandle final {
    public:
        ResourceHandle(T id) : _id(id) {};

        T id() const { return _id; }

    private:
        const T _id = NullId;
    };

    using MeshHandle = ResourceHandle<MeshId, NullMeshId>;
    using BufferHandle = ResourceHandle<BufferId, NullBufferId>;
    using TextureHandle = ResourceHandle<TextureId, NullTextureId>;
    using SamplerHandle = ResourceHandle<SamplerId, NullSamplerId>;
}
