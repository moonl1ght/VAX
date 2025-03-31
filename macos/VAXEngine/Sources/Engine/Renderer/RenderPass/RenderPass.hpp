//
// Created by Alexander Lakhonin on 28.03.2025.
//

#ifndef RenderPass_hpp
#define RenderPass_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "Texture.hpp"
#include "Scene.hpp"
#include "PipelineStateManager.hpp"

class RenderPass {
public:
  RenderPass(MTLStack* mtlStack)
  : _descriptor(MTL::RenderPassDescriptor::alloc()->init())
  , _mtlStack(mtlStack){ };
  virtual ~RenderPass();

  RenderPass(const RenderPass& rhs) = delete;
  RenderPass(const RenderPass&& rhs) = delete;

  RenderPass& operator=(RenderPass& rhs) = delete;
  RenderPass& operator=(RenderPass&& rhs) = delete;

  const MTL::RenderPassDescriptor& descriptor() const noexcept;

  virtual void draw(MTL::CommandBuffer* commandBuffer, Scene* scene, PipelineStateManager* pipelineStateManager) const noexcept = 0;
  virtual void updateRenderPassDescriptor(CA::MetalDrawable* drawable) noexcept;
  virtual void updateRenderPassDescriptor() noexcept;
  virtual void resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept;
protected:
  MTL::RenderPassDescriptor* _descriptor;
  MTLStack* _mtlStack;
};

#endif /* RenderPass_hpp */
