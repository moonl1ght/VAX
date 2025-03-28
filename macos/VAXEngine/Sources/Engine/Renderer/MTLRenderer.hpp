//
// Created by Alexander Lakhonin on 03.02.2025.
//

#ifndef MTLRenderer_hpp
#define MTLRenderer_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "MTLStack.hpp"
#include "Scene.hpp"
#include "VAXMathObjects.hpp"
#include "ForwardRenderPass.hpp"

class MTLRenderer
{
public:
  explicit MTLRenderer(MTLStack* _mtlStack, Scene* scene);
  virtual ~MTLRenderer();

  void draw(CA::MetalLayer* layer);
  void resize(const vax::Size viewSize, const vax::Size drawableSize);

private:
  MTLStack* _mtlStack;
  MTL::RenderPassDescriptor* _renderPassDescriptor;
  MTL::RenderPipelineState* _renderPipelineState;
  MTL::RenderPipelineState* _gizmoPipelineState;
  MTL::DepthStencilState* _depthStencilState;
  Scene* _scene;
  MTL::Texture* _depthTexture;
  ForwardRenderPass* _forwardRenderPass;

  void createDepthTexture(const vax::Size drawableSize);
  void updateRenderPassDescriptor(CA::MetalDrawable* drawable);
  void createRenderPipeline();
};

#endif /* MTLRenderer_hpp */
