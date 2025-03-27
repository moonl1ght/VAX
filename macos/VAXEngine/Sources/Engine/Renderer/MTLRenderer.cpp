//
// Created by Alexander Lakhonin on 03.02.2025.
//

#include "MTLRenderer.hpp"
#include <iostream>

#include "VertexDescriptor.hpp"
#include "ShaderTypes.h"
#include "RenderPipelineStateFactory.hpp"

using namespace std;
using namespace MTL;

MTLRenderer::MTLRenderer(MTLStack* mtlStack, Scene* scene)
: _mtlStack(mtlStack)
, _renderPassDescriptor(RenderPassDescriptor::alloc()->init())
, _scene(scene) {
  cout << "init renderer" << endl;
  createRenderPipeline();
};

MTLRenderer::~MTLRenderer() {
  cout << "deinit renderer" << endl;
  _renderPassDescriptor->release();
  _renderPassDescriptor = nullptr;
  _renderPipelineState->release();
  _renderPipelineState = nullptr;
  _depthStencilState->release();
  _depthStencilState = nullptr;
  _gizmoPipelineState->release();
  _gizmoPipelineState = nullptr;
};

void MTLRenderer::resize(const vax::Size viewSize, const vax::Size drawableSize) {
  createDepthTexture(drawableSize);
  _scene->camera.aspectRatio = drawableSize.whRatio();
}

void MTLRenderer::draw(CA::MetalLayer *layer) {
//  cout << "draw" << endl;
  CommandBuffer* commandBuffer = _mtlStack->commandQueue().commandBuffer();

  CA::MetalDrawable* drawable = layer->nextDrawable();
  updateRenderPassDescriptor(drawable);

  RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_renderPassDescriptor);

  renderCommandEncoder->setDepthStencilState(_depthStencilState);
//  renderCommandEncoder->setRenderPipelineState(_renderPipelineState);
  

  VertexUniforms vertexUniforms = { _scene->camera.viewMatrix(), _scene->camera.projectionMatrix() };
  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), kVertexUniformsBufferIndex);

  auto lights = _scene->lights();
  FragmentUniforms fragmentsUniforms = { _scene->camera.transform.position, (uint)lights.size() };
  renderCommandEncoder->setFragmentBytes(&fragmentsUniforms, sizeof(fragmentsUniforms), 3);
  renderCommandEncoder->setFragmentBytes(lights.data(), sizeof(Light) * lights.size(), 4);
  for (auto& model: _scene->models()) {
    model->draw(renderCommandEncoder, _renderPipelineState);
  }

  _scene->gizmo().draw(renderCommandEncoder, _gizmoPipelineState);

  renderCommandEncoder->endEncoding();

  commandBuffer->presentDrawable(drawable);
  commandBuffer->commit();
};

void MTLRenderer::updateRenderPassDescriptor(CA::MetalDrawable* drawable) {
  RenderPassColorAttachmentDescriptor* colorAttachment =
  _renderPassDescriptor->colorAttachments()->object(0);
  RenderPassDepthAttachmentDescriptor* depthAttachment = _renderPassDescriptor->depthAttachment();

  colorAttachment->setTexture(drawable->texture());
  colorAttachment->setLoadAction(LoadActionClear);
  colorAttachment->setClearColor(ClearColor(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f, 1.0));
  colorAttachment->setStoreAction(StoreActionStore);

  depthAttachment->setTexture(_depthTexture);
  depthAttachment->setLoadAction(MTL::LoadActionClear);
  depthAttachment->setStoreAction(MTL::StoreActionDontCare);
  depthAttachment->setClearDepth(1.0);
}

void MTLRenderer::createRenderPipeline() {
  _renderPipelineState = RenderPipelineStateFactory::createBaseRenderPipelineState(_mtlStack);
  DepthStencilDescriptor* depthStencilDescriptor = DepthStencilDescriptor::alloc()->init();
  depthStencilDescriptor->setDepthCompareFunction(CompareFunctionLessEqual);
  depthStencilDescriptor->setDepthWriteEnabled(true);
  _depthStencilState = _mtlStack->device().newDepthStencilState(depthStencilDescriptor);
  depthStencilDescriptor->release();

  _gizmoPipelineState = RenderPipelineStateFactory::createGizmoRenderPipelineState(_mtlStack);
}

void MTLRenderer::createDepthTexture(const vax::Size drawableSize) {
  if (_depthTexture != nullptr) {
    _depthTexture->release();
    _depthTexture = nullptr;
  }
  TextureDescriptor* depthTextureDescriptor = TextureDescriptor::alloc()->init();
  depthTextureDescriptor->setTextureType(TextureType2D);
  depthTextureDescriptor->setPixelFormat(PixelFormatDepth32Float);
  depthTextureDescriptor->setWidth(drawableSize.width);
  depthTextureDescriptor->setHeight(drawableSize.height);
  depthTextureDescriptor->setUsage(TextureUsageRenderTarget);

  _depthTexture = _mtlStack->device().newTexture(depthTextureDescriptor);

  depthTextureDescriptor->release();
}
