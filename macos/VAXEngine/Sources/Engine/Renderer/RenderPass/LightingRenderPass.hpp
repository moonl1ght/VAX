//
// Created by Alexander Lakhonin on 08.04.2025.
//

#ifndef LightingRenderPass_hpp
#define LightingRenderPass_hpp

#include <stdio.h>
#include "RenderPass.hpp"

class LightingRenderPass: public RenderPass {
public:
  vax::Texture* albedoTexture;
  vax::Texture* positionTexture;
  vax::Texture* normalTexture;

  using RenderPass::RenderPass;
  ~LightingRenderPass();

  void draw(MTL::CommandBuffer* commandBuffer, Scene* scene) const noexcept override;
  void resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept override;
  void updateRenderPassDescriptor(CA::MetalDrawable* drawable) noexcept override;

private:
  vax::Texture* _depthTexture;
};

#endif /* LightingRenderPass_hpp */
