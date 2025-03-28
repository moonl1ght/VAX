//
// Created by Alexander Lakhonin on 26.03.2025.
//

#ifndef RenderPipelineStateFactory_hpp
#define RenderPipelineStateFactory_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include "MTLStack.hpp"
#include "VertexDescriptor.hpp"

namespace RenderPipelineStateFactory {
  MTL::RenderPipelineState* createBaseRenderPipelineState(MTLStack* mtlStack);
  MTL::RenderPipelineState* createGizmoRenderPipelineState(MTLStack* mtlStack);
}

#endif /* RenderPipelineStateFactory_hpp */
