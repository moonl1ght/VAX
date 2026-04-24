#pragma once

#include "resourceUtils.h"

namespace vax {
    template<typename T>
    class ResourceHandle final {
    public:
        ResourceHandle(T id) : _id(id) {};

        T id() const { return _id; }

    private:
        const T _id = 0;
    };

    using MeshHandle = ResourceHandle<MeshId>;
    using BufferHandle = ResourceHandle<BufferId>;
}
