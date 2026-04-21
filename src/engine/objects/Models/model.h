#pragma once

#include "luna.h"
#include "transform.h"

class Model {
public:
    Transform transform;

    Model() { };
    virtual ~Model() { };
};