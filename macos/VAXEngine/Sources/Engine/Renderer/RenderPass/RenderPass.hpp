//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include "Texture.hpp"
#include "Scene.hpp"

class RenderPass {
public:
  RenderPass(): _descriptor(MTL::RenderPassDescriptor::alloc()->init()) { };
  virtual ~RenderPass();

  const MTL::RenderPassDescriptor& descriptor() const noexcept;
  virtual void draw(MTL::CommandBuffer* commandBuffer, Scene* scene) const noexcept = 0;
protected:
  MTL::RenderPassDescriptor* _descriptor;
};

#endif /* RenderPass_hpp */
