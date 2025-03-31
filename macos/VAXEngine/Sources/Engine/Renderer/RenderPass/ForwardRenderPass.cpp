//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "ForwardRenderPass.hpp"

using namespace MTL;

ForwardRenderPass::~ForwardRenderPass() {
  delete _depthTexture;
}

void ForwardRenderPass::draw(CommandBuffer *commandBuffer, Scene *scene, PipelineStateManager* pipelineStateManager) const noexcept {
  RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_descriptor);

  renderCommandEncoder->setDepthStencilState(pipelineStateManager->depthStencilState);
//  renderCommandEncoder->setRenderPipelineState(pipelineStateManager->renderPipelineState);

  VertexUniforms vertexUniforms = { scene->camera.viewMatrix(), scene->camera.projectionMatrix() };
  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), kVertexUniformsBufferIndex);

  auto lights = scene->lights();
  FragmentUniforms fragmentsUniforms = { scene->camera.transform.position, (uint)lights.size() };
  renderCommandEncoder->setFragmentBytes(&fragmentsUniforms, sizeof(fragmentsUniforms), 3);
  renderCommandEncoder->setFragmentBytes(lights.data(), sizeof(Light) * lights.size(), 4);
  for (auto& model: scene->models()) {
    model->draw(renderCommandEncoder, pipelineStateManager->renderPipelineStateForType(model->renderPipelineStateType));
  }

  scene->gizmo().draw(renderCommandEncoder, pipelineStateManager->gizmoPipelineState);

  renderCommandEncoder->endEncoding();
}

void ForwardRenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept {
  if (_depthTexture != nullptr) {
    delete _depthTexture;
  }
  TextureDescriptor* depthTextureDescriptor = TextureDescriptor::alloc()->init();
  depthTextureDescriptor->setTextureType(TextureType2D);
  depthTextureDescriptor->setPixelFormat(PixelFormatDepth32Float);
  depthTextureDescriptor->setWidth(drawableSize.width);
  depthTextureDescriptor->setHeight(drawableSize.height);
  depthTextureDescriptor->setUsage(TextureUsageRenderTarget);

  _depthTexture = new vax::Texture(depthTextureDescriptor, _mtlStack->device());

  depthTextureDescriptor->release();
}

void ForwardRenderPass::updateRenderPassDescriptor(CA::MetalDrawable* drawable) noexcept {
  RenderPassColorAttachmentDescriptor* colorAttachment = _descriptor->colorAttachments()->object(0);
  RenderPassDepthAttachmentDescriptor* depthAttachment = _descriptor->depthAttachment();

  colorAttachment->setTexture(drawable->texture());
  colorAttachment->setLoadAction(LoadActionClear);
  colorAttachment->setClearColor(ClearColor(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f, 1.0));
  colorAttachment->setStoreAction(StoreActionStore);

  depthAttachment->setTexture(&_depthTexture->texture());
  depthAttachment->setLoadAction(MTL::LoadActionClear);
  depthAttachment->setStoreAction(MTL::StoreActionDontCare);
  depthAttachment->setClearDepth(1.0);
}
