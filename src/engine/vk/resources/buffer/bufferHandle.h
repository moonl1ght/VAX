#pragma once

#include "bufferUtils.h"

namespace vax {
    class BufferHandle final {
    public:
        BufferHandle(BufferId id) : _id(id) {};

        BufferId id() const { return _id; }

    private:
        const BufferId _id = 0;
    };
}