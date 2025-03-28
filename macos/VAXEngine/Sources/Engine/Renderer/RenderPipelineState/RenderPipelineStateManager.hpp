//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef RenderPipelineStateManager_hpp
#define RenderPipelineStateManager_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

class RenderPipelineStateManager {
public:

  MTL::RenderPipelineState* renderPipelineState;
  MTL::RenderPipelineState* gizmoPipelineState;
  MTL::DepthStencilState* depthStencilState;

  RenderPipelineStateManager() { };
  virtual ~RenderPipelineStateManager();

  RenderPipelineStateManager(const RenderPipelineStateManager& rhs) = delete;
  RenderPipelineStateManager(const RenderPipelineStateManager&& rhs) = delete;

  RenderPipelineStateManager& operator=(RenderPipelineStateManager& rhs) = delete;
  RenderPipelineStateManager& operator=(RenderPipelineStateManager&& rhs) = delete;
};

#endif /* RenderPipelineStateManager_hpp */
