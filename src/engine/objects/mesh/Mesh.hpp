#ifndef Mesh_hpp
#define Mesh_hpp

#include "VKStack.hpp"
#include "Vertex.h"
#include "Buffer.hpp"
#include "luna.h"
#include <iostream>

class Mesh {
public:
    Buffer* vertexBuffer;
    std::vector<Vertex> vertices;

    Mesh() {};
    ~Mesh() {};

    void prepareForRender(VKStack *vkStack);
    void cleanup(VKStack *vkStack);
};
#endif