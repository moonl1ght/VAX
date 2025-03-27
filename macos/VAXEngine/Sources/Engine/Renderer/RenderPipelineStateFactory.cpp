//
// Created by Alexander Lakhonin on 26.03.2025.
//

#include "RenderPipelineStateFactory.hpp"
#include <iostream>

using namespace MTL;

MTL::RenderPipelineState* RenderPipelineStateFactory::createBaseRenderPipelineState(MTLStack *mtlStack) {
  Function* vertexFunction = mtlStack->library().newFunction(NS::String::string("basicVertex", NS::ASCIIStringEncoding));
  Function* fragmentFunction = mtlStack->library().newFunction(NS::String::string("basicFragmentWithPBR", NS::ASCIIStringEncoding));

  RenderPipelineDescriptor* renderPipelineDescriptor = RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);
  vax::VertexDescriptor vertexDescriptor = vax::VertexDescriptor::createSimpleVertexDescriptor();
  renderPipelineDescriptor->setVertexDescriptor(&vertexDescriptor.vertexDescriptor());
  assert(renderPipelineDescriptor);
  PixelFormat pixelFormat = PixelFormatBGRA8Unorm;
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
  //  renderPipelineDescriptor->setSampleCount(4);
  renderPipelineDescriptor->setLabel(NS::String::string("Render Pipeline", NS::ASCIIStringEncoding));
  renderPipelineDescriptor->setDepthAttachmentPixelFormat(PixelFormatDepth32Float);
  //  renderPipelineDescriptor->setTessellationOutputWindingOrder(MTL::WindingCounterClockwise);

  NS::Error* error;
  MTL::RenderPipelineState* renderPipelineState = mtlStack->device().newRenderPipelineState(renderPipelineDescriptor, &error);
  if (renderPipelineState == nullptr) {
    std::cout << "Error creating render pipeline state: " << error << std::endl;
    assert(renderPipelineState);
  }
  renderPipelineDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  return renderPipelineState;
}

MTL::RenderPipelineState* RenderPipelineStateFactory::createGizmoRenderPipelineState(MTLStack *mtlStack) {
  Function* vertexFunction = mtlStack->library().newFunction(NS::String::string("debugVertex", NS::ASCIIStringEncoding));
  Function* fragmentFunction = mtlStack->library().newFunction(NS::String::string("debugFragment", NS::ASCIIStringEncoding));

  RenderPipelineDescriptor* renderPipelineDescriptor = RenderPipelineDescriptor::alloc()->init();
  renderPipelineDescriptor->setVertexFunction(vertexFunction);
  renderPipelineDescriptor->setFragmentFunction(fragmentFunction);
  vax::VertexDescriptor vertexDescriptor = vax::VertexDescriptor::createPrmitiveVertexDescriptor();
  renderPipelineDescriptor->setVertexDescriptor(&vertexDescriptor.vertexDescriptor());
  assert(renderPipelineDescriptor);
  PixelFormat pixelFormat = PixelFormatBGRA8Unorm;
  renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
  //  renderPipelineDescriptor->setSampleCount(4);
  renderPipelineDescriptor->setLabel(NS::String::string("Gizmo Render Pipeline", NS::ASCIIStringEncoding));
  renderPipelineDescriptor->setDepthAttachmentPixelFormat(PixelFormatDepth32Float);
  //  renderPipelineDescriptor->setTessellationOutputWindingOrder(MTL::WindingCounterClockwise);

  NS::Error* error;
  MTL::RenderPipelineState* renderPipelineState = mtlStack->device().newRenderPipelineState(renderPipelineDescriptor, &error);
  if (renderPipelineState == nullptr) {
    std::cout << "Error creating render pipeline state: " << error->debugDescription()->cString(NS::ASCIIStringEncoding) << std::endl;
    assert(renderPipelineState);
  }
  renderPipelineDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  return renderPipelineState;
}
