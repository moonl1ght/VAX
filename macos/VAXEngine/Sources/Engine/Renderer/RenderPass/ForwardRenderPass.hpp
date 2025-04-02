//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef ForwardRenderPass_hpp
#define ForwardRenderPass_hpp

#include <stdio.h>
#include "RenderPass.hpp"

class ForwardRenderPass: public RenderPass {
public:
  vax::Texture* shadowTexture;

  using RenderPass::RenderPass;
  ~ForwardRenderPass();

  void draw(MTL::CommandBuffer* commandBuffer, Scene* scene) const noexcept override;
  void resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept override;
  void updateRenderPassDescriptor(CA::MetalDrawable* drawable) noexcept override;

private:
  vax::Texture* _depthTexture;
};

#endif /* ForwardRenderPass_hpp */
