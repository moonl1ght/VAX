//
// Created by Alexander Lakhonin on 31.03.2025.
//

#include "ShadowRenderPass.hpp"

vax::Texture& ShadowRenderPass::shadowTexture() const noexcept {
  return *_shadowTexture;
}

ShadowRenderPass::~ShadowRenderPass() {
  delete _shadowTexture;
  _shadowTexture = nullptr;
}

void ShadowRenderPass::draw(MTL::CommandBuffer *commandBuffer, Scene *scene) const noexcept {
  MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_descriptor);

  renderCommandEncoder->setDepthStencilState(_pipelineStateManager->depthStencilState);
  renderCommandEncoder->setRenderPipelineState(_pipelineStateManager->shadowPipelineState);

  ShadowVertexUniforms shadowVertexUniforms = {
    .shadowProjectionMatrix = scene->shadowCamera.projectionMatrix(),
    .shadowViewMatrix = scene->shadowCamera.viewMatrix(),
  };
  renderCommandEncoder->setVertexBytes(&shadowVertexUniforms, shadowVertexUniforms.size(), kShadowUniformsBufferIndex);

//  VertexUniforms vertexUniforms = { scene->camera.viewMatrix(), scene->shadowCamera.projectionMatrix() };
//  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), kVertexUniformsBufferIndex);

  for (auto& model: scene->models()) {
    model->draw(renderCommandEncoder, nullptr, Mesh::RenderingMode::meshOnly);
  }

  renderCommandEncoder->endEncoding();
}

void ShadowRenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept { }

void ShadowRenderPass::updateRenderPassDescriptor() noexcept {
  MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = _descriptor->depthAttachment();

  depthAttachment->setTexture(&_shadowTexture->texture());
  depthAttachment->setLoadAction(MTL::LoadActionClear);
  depthAttachment->setStoreAction(MTL::StoreActionStore);
}
