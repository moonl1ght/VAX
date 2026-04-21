#pragma once

#include "luna.h"
#include "transform.h"

namespace vax::objects {
    class Model {
    public:
        vax::math::Transform transform;

        Model() {};
        virtual ~Model() {};
    };
}