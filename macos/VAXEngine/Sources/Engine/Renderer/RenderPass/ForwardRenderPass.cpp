//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "ForwardRenderPass.hpp"

using namespace MTL;

ForwardRenderPass::~ForwardRenderPass() {
  delete _depthTexture;
  _depthTexture = nullptr;
}

void ForwardRenderPass::draw(CommandBuffer *commandBuffer, Scene *scene) const noexcept {
  RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_descriptor);

  renderCommandEncoder->setDepthStencilState(_pipelineStateManager->depthStencilState);

  VertexUniforms vertexUniforms = { scene->camera.viewMatrix(), scene->camera.projectionMatrix() };
  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), kVertexUniformsBufferIndex);
  renderCommandEncoder->setLabel(NS::String::string("forward render pass command encoder", NS::ASCIIStringEncoding));

  ShadowVertexUniforms shadowVertexUniforms = {
    .shadowProjectionMatrix = scene->shadowCamera.projectionMatrix(),
    .shadowViewMatrix = scene->shadowCamera.viewMatrix(),
  };
  renderCommandEncoder->setVertexBytes(&shadowVertexUniforms, shadowVertexUniforms.size(), kShadowUniformsBufferIndex);

  auto lights = scene->lights();
  FragmentUniforms fragmentsUniforms = { scene->camera.transform.position, (uint)lights.size() };
  renderCommandEncoder->setFragmentBytes(&fragmentsUniforms, sizeof(fragmentsUniforms), kFragmentUniformsIndex);
  renderCommandEncoder->setFragmentBytes(lights.data(), sizeof(Light) * lights.size(), kLightIndex);
  if (shadowTexture) {
    renderCommandEncoder->setFragmentTexture(&shadowTexture->texture(), kTextureShadowIndex);
  }
  for (auto& model: scene->models()) {
    model->draw(renderCommandEncoder, _pipelineStateManager->renderPipelineStateForType(model->renderPipelineStateType, false));
  }

  scene->gizmo().draw(renderCommandEncoder, _pipelineStateManager->gizmoPipelineState);

  renderCommandEncoder->endEncoding();
}

void ForwardRenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept {
  if (_depthTexture != nullptr) {
    delete _depthTexture;
  }
//  TextureDescriptor* depthTextureDescriptor = TextureDescriptor::alloc()->init();
//  depthTextureDescriptor->setTextureType(TextureType2D);
//  depthTextureDescriptor->setPixelFormat(PixelFormatDepth32Float);
//  depthTextureDescriptor->setWidth(drawableSize.width);
//  depthTextureDescriptor->setHeight(drawableSize.height);
//  depthTextureDescriptor->setUsage(TextureUsageRenderTarget);

  _depthTexture = new vax::Texture(viewSize, MTL::PixelFormatDepth32Float, _mtlStack->device());

//  depthTextureDescriptor->release();
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
