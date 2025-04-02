//
// Created by Alexander Lakhonin on 31.03.2025.
//

#ifndef ShadowRenderPass_hpp
#define ShadowRenderPass_hpp

#include <stdio.h>
#include "RenderPass.hpp"

class ShadowRenderPass: public RenderPass {
public:
  ShadowRenderPass(MTLStack* mtlStack, PipelineStateManager* pipelineStateManager): RenderPass(mtlStack, pipelineStateManager) {
    MTL::TextureDescriptor* shadowTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
    shadowTextureDescriptor->setTextureType(MTL::TextureType2D);
    shadowTextureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
    shadowTextureDescriptor->setWidth(2048);
    shadowTextureDescriptor->setHeight(2048);
    shadowTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);

    _shadowTexture = new vax::Texture(shadowTextureDescriptor, mtlStack->device());

    shadowTextureDescriptor->release();
  };
  ~ShadowRenderPass();

  void draw(MTL::CommandBuffer* commandBuffer, Scene* scene) const noexcept override;
  void resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept override;
  void updateRenderPassDescriptor() noexcept override;

private:
  vax::Texture* _shadowTexture;
};

#endif /* ShadowRenderPass_hpp */
