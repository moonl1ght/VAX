//
// Created by Alexander Lakhonin on 03.02.2025.
//

#include "MTLRenderer.hpp"
#include <iostream>

#include "Primitives.hpp"
#include "VertexDescriptor.hpp"

using namespace std;
using namespace MTL;

MTLRenderer::MTLRenderer(MTLStack* mtlStack)
: _mtlStack(mtlStack)
, _renderPassDescriptor(RenderPassDescriptor::alloc()->init())
, _triangle(Primitives::createRGBTriangle(mtlStack->device())){
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

void MTLRenderer::draw(CA::MetalLayer *layer) {
//  cout << "draw" << endl;
  CommandBuffer* commandBuffer = _mtlStack->commandQueue().commandBuffer();

  CA::MetalDrawable* drawable = layer->nextDrawable();
  updateRenderPassDescriptor(drawable);

  RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_renderPassDescriptor);

//  renderCommandEncoder->setDepthStencilState(_depthStencilState);
  renderCommandEncoder->setRenderPipelineState(_renderPipelineState);

  _triangle->draw(renderCommandEncoder);

  renderCommandEncoder->endEncoding();

//  const Drawable* drawble;
  commandBuffer->presentDrawable(drawable);
  commandBuffer->commit();
};

void MTLRenderer::updateRenderPassDescriptor(CA::MetalDrawable* drawable) {
  RenderPassColorAttachmentDescriptor* colorAttachment =
  _renderPassDescriptor->colorAttachments()->object(0);

  colorAttachment->setTexture(drawable->texture());
  colorAttachment->setLoadAction(LoadActionClear);
  colorAttachment->setClearColor(ClearColor(41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0));
  colorAttachment->setStoreAction(StoreActionStore);
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
  renderPipelineDescriptor->setDepthAttachmentPixelFormat(PixelFormatInvalid);
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
