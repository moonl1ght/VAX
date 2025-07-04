#ifndef Renderer_hpp
#define Renderer_hpp

#include "luna.h"
#include "VKStack.hpp"
#include "DescriptorSetManager.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render();
};

#endif