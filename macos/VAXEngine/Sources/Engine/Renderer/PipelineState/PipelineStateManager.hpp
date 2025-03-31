//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef PipelineStateManager_hpp
#define PipelineStateManager_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include "PipelineStateFactory.hpp"

class PipelineStateManager {
public:

  MTL::RenderPipelineState* renderPipelineState;
  MTL::RenderPipelineState* gizmoPipelineState;
  MTL::DepthStencilState* depthStencilState;

  PipelineStateManager(MTLStack* mtlStack) {
    renderPipelineState = PipelineStateFactory::createBaseRenderPipelineState(mtlStack);
    gizmoPipelineState = PipelineStateFactory::createGizmoRenderPipelineState(mtlStack);
    depthStencilState = PipelineStateFactory::createDepthStencilState(mtlStack);
  };
  virtual ~PipelineStateManager();

  PipelineStateManager(const PipelineStateManager& rhs) = delete;
  PipelineStateManager(const PipelineStateManager&& rhs) = delete;

  PipelineStateManager& operator=(PipelineStateManager& rhs) = delete;
  PipelineStateManager& operator=(PipelineStateManager&& rhs) = delete;
};

#endif /* PipelineStateManager_hpp */
