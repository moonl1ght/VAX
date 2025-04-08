//
// Created by Alexander Lakhonin on 04.04.2025.
//

#include "GBufferRenderPass.hpp"

vax::Texture& GBufferRenderPass::albedoTexture() const noexcept {
  return *_albedoTexture;
}

vax::Texture& GBufferRenderPass::positionTexture() const noexcept {
  return *_positionTexture;
}

vax::Texture& GBufferRenderPass::normalTexture() const noexcept {
  return *_normalTexture;
}

GBufferRenderPass::~GBufferRenderPass() {
  delete _albedoTexture;
  _albedoTexture = nullptr;
  delete _positionTexture;
  _positionTexture = nullptr;
  delete _normalTexture;
  _normalTexture = nullptr;
  delete _depthTexture;
  _depthTexture = nullptr;
}

void GBufferRenderPass::draw(MTL::CommandBuffer *commandBuffer, Scene *scene) const noexcept {
//  auto textures = (std::vector<vax::Texture *>) {_albedoTexture, _normalTexture, _positionTexture};
  MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_descriptor);
//
  renderCommandEncoder->setDepthStencilState(_pipelineStateManager->depthStencilState);
//  renderCommandEncoder->setRenderPipelineState(_pipelineStateManager->shadowPipelineState);
  renderCommandEncoder->setLabel(NS::String::string("g buffer render pass command encoder", NS::ASCIIStringEncoding));
//
  ShadowVertexUniforms shadowVertexUniforms = {
    .shadowProjectionMatrix = scene->shadowCamera.projectionMatrix(),
    .shadowViewMatrix = scene->shadowCamera.viewMatrix(),
  };
  renderCommandEncoder->setVertexBytes(&shadowVertexUniforms, shadowVertexUniforms.size(), kShadowUniformsBufferIndex);

  VertexUniforms vertexUniforms = { scene->camera.viewMatrix(), scene->camera.projectionMatrix() };
  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), kVertexUniformsBufferIndex);

  if (shadowTexture) {
    renderCommandEncoder->setFragmentTexture(&shadowTexture->texture(), kTextureShadowIndex);
  }
  for (auto& model: scene->models()) {
    model->draw(renderCommandEncoder, _pipelineStateManager->renderPipelineStateForType(model->renderPipelineStateType, true));
  }
//
  renderCommandEncoder->endEncoding();
}

void GBufferRenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept {
  if (_depthTexture != nullptr) {
    delete _depthTexture;
  }
  _depthTexture = new vax::Texture(drawableSize, MTL::PixelFormatDepth32Float, _mtlStack->device());
  if (_albedoTexture != nullptr) {
    delete _albedoTexture;
  }
  _albedoTexture = new vax::Texture(drawableSize, MTL::PixelFormatBGRA8Unorm, _mtlStack->device());
  if (_normalTexture != nullptr) {
    delete _normalTexture;
  }
  _normalTexture = new vax::Texture(drawableSize, MTL::PixelFormatRGBA16Float, _mtlStack->device());
  if (_positionTexture != nullptr) {
    delete _positionTexture;
  }
  _positionTexture = new vax::Texture(drawableSize, MTL::PixelFormatRGBA16Float, _mtlStack->device());
}

void GBufferRenderPass::updateRenderPassDescriptor() noexcept {
  auto textures = (std::vector<vax::Texture *>) {_albedoTexture, _normalTexture, _positionTexture};

  unsigned i = 1;
  for (vax::Texture* texture : textures) {
    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = _descriptor->colorAttachments()->object(i);

    colorAttachment->setTexture(&texture->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setStoreAction(MTL::StoreActionStore);
    colorAttachment->setClearColor(MTL::ClearColor(0.73, 0.92, 1, 1));
    ++i;
  }

  MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = _descriptor->depthAttachment();

  depthAttachment->setTexture(&_depthTexture->texture());
  depthAttachment->setLoadAction(MTL::LoadActionClear);
  depthAttachment->setStoreAction(MTL::StoreActionDontCare);
}
