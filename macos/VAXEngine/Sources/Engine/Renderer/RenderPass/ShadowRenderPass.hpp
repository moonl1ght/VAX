//
// Created by Alexander Lakhonin on 31.03.2025.
//

#ifndef ShadowRenderPass_hpp
#define ShadowRenderPass_hpp

#include <stdio.h>
#include "RenderPass.hpp"

class ShadowRenderPass: RenderPass {
public:
  using RenderPass::RenderPass;
  ~ShadowRenderPass();

  void draw(MTL::CommandBuffer* commandBuffer, Scene* scene, PipelineStateManager* pipelineStateManager) const noexcept override;
  void resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept override;
};

#endif /* ShadowRenderPass_hpp */
