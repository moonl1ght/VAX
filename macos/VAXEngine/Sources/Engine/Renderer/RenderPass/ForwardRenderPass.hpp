//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef ForwardRenderPass_hpp
#define ForwardRenderPass_hpp

#include <stdio.h>
#include "RenderPass.hpp"

class ForwardRenderPass: RenderPass {
public:
  void draw(MTL::CommandBuffer* commandBuffer, Scene* scene) const noexcept override;
private:
  Texture* depthTexture;
};

#endif /* ForwardRenderPass_hpp */
