//
// Created by Alexander Lakhonin on 03.02.2025.
//

#include "MTLRenderer.hpp"
#include <iostream>

#include "VertexDescriptor.hpp"
#include "ShaderTypes.h"

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
};

void MTLRenderer::resize(const VAXSize viewSize, const VAXSize drawableSize) {
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
  renderCommandEncoder->setRenderPipelineState(_renderPipelineState);

  VertexUniforms vertexUniforms = { _scene->camera.viewMatrix(), _scene->camera.projectionMatrix() };
  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), 10);
  for (auto& model: _scene->models()) {
    model->mesh().draw(renderCommandEncoder);
  }

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
  colorAttachment->setClearColor(ClearColor(1/255.0f, 0/255.0f, 48.0f/255.0f, 1.0));
  colorAttachment->setStoreAction(StoreActionStore);

  depthAttachment->setTexture(_depthTexture);
  depthAttachment->setLoadAction(MTL::LoadActionClear);
  depthAttachment->setStoreAction(MTL::StoreActionDontCare);
  depthAttachment->setClearDepth(1.0);
}

void MTLRenderer::createRenderPipeline() {
  Function* vertexFunction = _mtlStack->library().newFunction(NS::String::string("basicVertex", NS::ASCIIStringEncoding));
  Function* fragmentFunction = _mtlStack->library().newFunction(NS::String::string("basicFragment", NS::ASCIIStringEncoding));

  RenderPipelineDescriptor* renderPipelineDescriptor = RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);
  VAX::VertexDescriptor vertexDescriptor = VAX::VertexDescriptor::createSimpleVertexDescriptor();
  renderPipelineDescriptor->setVertexDescriptor(&vertexDescriptor.vertexDescriptor());
  assert(renderPipelineDescriptor);
  PixelFormat pixelFormat = PixelFormatBGRA8Unorm;
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
//  renderPipelineDescriptor->setSampleCount(4);
  renderPipelineDescriptor->setLabel(NS::String::string("Render Pipeline", NS::ASCIIStringEncoding));
  renderPipelineDescriptor->setDepthAttachmentPixelFormat(PixelFormatDepth32Float);
//  renderPipelineDescriptor->setTessellationOutputWindingOrder(MTL::WindingCounterClockwise);

  NS::Error* error;
  _renderPipelineState = _mtlStack->device().newRenderPipelineState(renderPipelineDescriptor, &error);
  if (_renderPipelineState == nullptr) {
    std::cout << "Error creating render pipeline state: " << error << std::endl;
    assert(_renderPipelineState);
  }

  DepthStencilDescriptor* depthStencilDescriptor = DepthStencilDescriptor::alloc()->init();
  depthStencilDescriptor->setDepthCompareFunction(CompareFunctionLessEqual);
  depthStencilDescriptor->setDepthWriteEnabled(true);
  _depthStencilState = _mtlStack->device().newDepthStencilState(depthStencilDescriptor);

  depthStencilDescriptor->release();
  renderPipelineDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
}

void MTLRenderer::createDepthTexture(const VAXSize drawableSize) {
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
