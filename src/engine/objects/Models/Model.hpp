#ifndef Model_hpp
#define Model_hpp

#include "luna.h"
#include "Transform.hpp"

class Model {
public:
    Transform transform;

    Model() { };
    virtual ~Model() { };
};

#endif