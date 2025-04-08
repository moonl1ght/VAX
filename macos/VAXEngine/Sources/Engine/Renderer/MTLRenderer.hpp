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
#include "ShadowRenderPass.hpp"
#include "GBufferRenderPass.hpp"
#include "LightingRenderPass.hpp"

class MTLRenderer
{
public:
  explicit MTLRenderer(MTLStack* _mtlStack, Scene* scene);
  virtual ~MTLRenderer();

  void draw(CA::MetalLayer* layer);
  void resize(const vax::Size viewSize, const vax::Size drawableSize);

private:
  MTLStack* _mtlStack;
  PipelineStateManager* _pipelineStateManager;
  ForwardRenderPass* _forwardRenderPass;
  ShadowRenderPass* _shadowRenderPass;
  GBufferRenderPass* _gBufferRenderPass;
  LightingRenderPass* _lightingRenderPass;
  Scene* _scene;
};

#endif /* MTLRenderer_hpp */
