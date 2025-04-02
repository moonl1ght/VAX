//
// Created by Alexander Lakhonin on 26.03.2025.
//

#ifndef PipelineStateFactory_hpp
#define PipelineStateFactory_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include "MTLStack.hpp"
#include "VertexDescriptor.hpp"

namespace PipelineStateFactory {
  MTL::RenderPipelineState* createBaseRenderPipelineState(MTLStack* mtlStack);
  MTL::RenderPipelineState* createGizmoRenderPipelineState(MTLStack* mtlStack);
  MTL::RenderPipelineState* createPrimitiveRenderPipelineState(MTLStack* mtlStack);
  MTL::RenderPipelineState* createShadowRenderPipelineState(MTLStack* mtlStack);

  MTL::DepthStencilState* createDepthStencilState(MTLStack* mtlStack);
}

#endif /* PipelineStateFactory_hpp */
