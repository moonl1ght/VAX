#ifndef Mesh_hpp
#define Mesh_hpp

#include "VKStack.hpp"
#include "Vertex.h"
#include "luna.h"
#include <iostream>

class Mesh {
public:
    VkBuffer vertexBuffer;
    std::vector<Vertex> vertices;

    Mesh() {};
    ~Mesh() {};

    void prepareForRender(VKStack *vkStack);
    void cleanup(VKStack *vkStack);

private:
    VkDeviceMemory vertexBufferMemory;
};
#endif