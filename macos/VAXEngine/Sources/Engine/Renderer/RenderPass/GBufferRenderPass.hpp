//
// Created by Alexander Lakhonin on 04.04.2025.
//

#ifndef GBufferRenderPass_hpp
#define GBufferRenderPass_hpp

#include <stdio.h>
#include "RenderPass.hpp"

class GBufferRenderPass: public RenderPass {
public:
  vax::Texture* shadowTexture;

  vax::Texture& albedoTexture() const noexcept;
  vax::Texture& positionTexture() const noexcept;
  vax::Texture& normalTexture() const noexcept;

  using RenderPass::RenderPass;
  ~GBufferRenderPass();

  void draw(MTL::CommandBuffer* commandBuffer, Scene* scene) const noexcept override;
  void resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept override;
  void updateRenderPassDescriptor() noexcept override;

private:
  vax::Texture* _albedoTexture;
  vax::Texture* _positionTexture;
  vax::Texture* _normalTexture;
  vax::Texture* _depthTexture;
};

#endif /* GBufferRenderPass_hpp */
