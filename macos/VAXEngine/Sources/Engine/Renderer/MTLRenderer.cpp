//
// Created by Alexander Lakhonin on 03.02.2025.
//

#include "MTLRenderer.hpp"
#include <iostream>

using namespace std;
using namespace MTL;

MTLRenderer::MTLRenderer(MTLStack* mtlStack, Scene* scene)
: _mtlStack(mtlStack)
, _forwardRenderPass(new ForwardRenderPass(mtlStack))
, _pipelineStateManager(new PipelineStateManager(mtlStack))
, _scene(scene) {
  cout << "init renderer" << endl;
};

MTLRenderer::~MTLRenderer() {
  cout << "deinit renderer" << endl;
  delete _forwardRenderPass;
  _forwardRenderPass = nullptr;
  delete _pipelineStateManager;
  _pipelineStateManager = nullptr;
};

void MTLRenderer::resize(const vax::Size viewSize, const vax::Size drawableSize) {
  _forwardRenderPass->resize(viewSize, drawableSize);
  _scene->camera.aspectRatio = drawableSize.whRatio();
}

void MTLRenderer::draw(CA::MetalLayer *layer) {
//  cout << "draw" << endl;
  CommandBuffer* commandBuffer = _mtlStack->commandQueue().commandBuffer();

  CA::MetalDrawable* drawable = layer->nextDrawable();
  _forwardRenderPass->updateRenderPassDescriptor(drawable);

  _forwardRenderPass->draw(commandBuffer, _scene, _pipelineStateManager);

  commandBuffer->presentDrawable(drawable);
  commandBuffer->commit();
};
