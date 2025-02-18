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

class MTLRenderer
{
public:
  explicit MTLRenderer(MTLStack* _mtlStack, Scene* scene);
  virtual ~MTLRenderer();

  void draw(CA::MetalLayer* layer);

private:
  MTLStack* _mtlStack;
  MTL::RenderPassDescriptor* _renderPassDescriptor;
  MTL::RenderPipelineState* _renderPipelineState;
  MTL::DepthStencilState* _depthStencilState;
  Scene* _scene;

  void updateRenderPassDescriptor(CA::MetalDrawable* drawable);
  void createRenderPipeline();
};

#endif /* MTLRenderer_hpp */
