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
  enum class RenderPipelineStateType {
    base, primitive, gizmo
  };

  MTL::RenderPipelineState* renderPipelineState;
  MTL::RenderPipelineState* primitivePipelineState;
  MTL::RenderPipelineState* gizmoPipelineState;
  MTL::RenderPipelineState* shadowPipelineState;

  MTL::DepthStencilState* depthStencilState;

  PipelineStateManager(MTLStack* mtlStack) {
    renderPipelineState = PipelineStateFactory::createBaseRenderPipelineState(mtlStack);
    gizmoPipelineState = PipelineStateFactory::createGizmoRenderPipelineState(mtlStack);
    primitivePipelineState = PipelineStateFactory::createPrimitiveRenderPipelineState(mtlStack);
    shadowPipelineState = PipelineStateFactory::createShadowRenderPipelineState(mtlStack);

    depthStencilState = PipelineStateFactory::createDepthStencilState(mtlStack);
  };
  virtual ~PipelineStateManager();

  PipelineStateManager(const PipelineStateManager& rhs) = delete;
  PipelineStateManager(const PipelineStateManager&& rhs) = delete;

  PipelineStateManager& operator=(PipelineStateManager& rhs) = delete;
  PipelineStateManager& operator=(PipelineStateManager&& rhs) = delete;

  MTL::RenderPipelineState* renderPipelineStateForType(RenderPipelineStateType type) const noexcept;
};

#endif /* PipelineStateManager_hpp */
