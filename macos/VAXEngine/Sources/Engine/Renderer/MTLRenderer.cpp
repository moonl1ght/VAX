//
// Created by Alexander Lakhonin on 03.02.2025.
//

#include "MTLRenderer.hpp"
#include <iostream>

using namespace std;
using namespace MTL;

MTLRenderer::MTLRenderer(MTLStack* mtlStack, Scene* scene)
: _mtlStack(mtlStack)
, _pipelineStateManager(new PipelineStateManager(mtlStack))
, _forwardRenderPass(new ForwardRenderPass(mtlStack, _pipelineStateManager))
, _shadowRenderPass(new ShadowRenderPass(mtlStack, _pipelineStateManager))
, _gBufferRenderPass(new GBufferRenderPass(mtlStack, _pipelineStateManager))
, _lightingRenderPass(new LightingRenderPass(mtlStack, _pipelineStateManager))
, _scene(scene) {
  cout << "init renderer" << endl;
};

MTLRenderer::~MTLRenderer() {
  cout << "deinit renderer" << endl;
  delete _forwardRenderPass;
  _forwardRenderPass = nullptr;
  delete _shadowRenderPass;
  _shadowRenderPass = nullptr;
  delete _pipelineStateManager;
  _pipelineStateManager = nullptr;
  delete _gBufferRenderPass;
  _gBufferRenderPass = nullptr;
  delete _lightingRenderPass;
  _lightingRenderPass = nullptr;
};

void MTLRenderer::resize(const vax::Size viewSize, const vax::Size drawableSize) {
  _forwardRenderPass->resize(viewSize, drawableSize);
  _gBufferRenderPass->resize(viewSize, drawableSize);
  _lightingRenderPass->resize(viewSize, drawableSize);
  _scene->camera.aspectRatio = drawableSize.whRatio();
}

void MTLRenderer::draw(CA::MetalLayer *layer) {
//  cout << "draw" << endl;
  CommandBuffer* commandBuffer = _mtlStack->commandQueue().commandBuffer();

  _shadowRenderPass->updateRenderPassDescriptor();
  _shadowRenderPass->draw(commandBuffer, _scene);

  bool isDeferredRendering = true;
  if (isDeferredRendering) {
    _gBufferRenderPass->shadowTexture = &_shadowRenderPass->shadowTexture();
    _gBufferRenderPass->updateRenderPassDescriptor();
    _gBufferRenderPass->draw(commandBuffer, _scene);

    _lightingRenderPass->albedoTexture = &_gBufferRenderPass->albedoTexture();
    _lightingRenderPass->positionTexture = &_gBufferRenderPass->positionTexture();
    _lightingRenderPass->normalTexture = &_gBufferRenderPass->normalTexture();
    CA::MetalDrawable* drawable = layer->nextDrawable();
    _lightingRenderPass->updateRenderPassDescriptor(drawable);
    _lightingRenderPass->draw(commandBuffer, _scene);
    commandBuffer->presentDrawable(drawable);
  } else {
    _forwardRenderPass->shadowTexture = &_shadowRenderPass->shadowTexture();
    CA::MetalDrawable* drawable = layer->nextDrawable();
    _forwardRenderPass->updateRenderPassDescriptor(drawable);
    _forwardRenderPass->draw(commandBuffer, _scene);
    commandBuffer->presentDrawable(drawable);
  }

  commandBuffer->commit();
};
