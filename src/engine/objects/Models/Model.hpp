#ifndef Model_hpp
#define Model_hpp

#include "luna.h"
#include "Transform.hpp"
#include "Mesh.hpp"

class Model {
public:
    Transform transform;

    Model();
    ~Model();
};

#endif