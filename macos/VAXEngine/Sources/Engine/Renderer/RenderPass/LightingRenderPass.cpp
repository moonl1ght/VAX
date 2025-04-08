//
// Created by Alexander Lakhonin on 08.04.2025.
//

#include "LightingRenderPass.hpp"

LightingRenderPass::~LightingRenderPass() {
  delete _depthTexture;
  _depthTexture = nullptr;
}

void LightingRenderPass::draw(MTL::CommandBuffer *commandBuffer, Scene *scene) const noexcept {
  MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_descriptor);
  //
  renderCommandEncoder->setDepthStencilState(_pipelineStateManager->depthStencilState);
  renderCommandEncoder->setRenderPipelineState(_pipelineStateManager->sunlightPipelineState);
  renderCommandEncoder->setLabel(NS::String::string("lighting render pass command encoder", NS::ASCIIStringEncoding));
  //
  auto lights = scene->lights();
  FragmentUniforms fragmentsUniforms = { scene->camera.transform.position, (uint)lights.size() };
  renderCommandEncoder->setFragmentBytes(&fragmentsUniforms, sizeof(fragmentsUniforms), kFragmentUniformsIndex);
  renderCommandEncoder->setFragmentBytes(lights.data(), sizeof(Light) * lights.size(), kLightIndex);

  renderCommandEncoder->setFragmentTexture(&albedoTexture->texture(), kBaseColorTextureIndex);
  renderCommandEncoder->setFragmentTexture(&normalTexture->texture(), kNormalTextureIndex);
  renderCommandEncoder->setFragmentTexture(&positionTexture->texture(), kPositionTextureIndex);

//  if (shadowTexture) {
//    renderCommandEncoder->setFragmentTexture(&shadowTexture->texture(), kTextureShadowIndex);
//  }
//  for (auto& model: scene->models()) {
//    model->draw(renderCommandEncoder, _pipelineStateManager->renderPipelineStateForType(model->renderPipelineStateType, true));
//  }
  renderCommandEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), 6);
  //
  renderCommandEncoder->endEncoding();
}

void LightingRenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept {
  if (_depthTexture != nullptr) {
    delete _depthTexture;
  }
  _depthTexture = new vax::Texture(viewSize, MTL::PixelFormatDepth32Float, _mtlStack->device());
}

void LightingRenderPass::updateRenderPassDescriptor(CA::MetalDrawable* drawable) noexcept {
  MTL::RenderPassColorAttachmentDescriptor* colorAttachment = _descriptor->colorAttachments()->object(0);
  MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = _descriptor->depthAttachment();

  colorAttachment->setTexture(drawable->texture());
  colorAttachment->setLoadAction(MTL::LoadActionClear);
  colorAttachment->setClearColor(MTL::ClearColor(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f, 1.0));
  colorAttachment->setStoreAction(MTL::StoreActionStore);

//  depthAttachment->set
  depthAttachment->setTexture(&_depthTexture->texture());
  depthAttachment->setLoadAction(MTL::LoadActionClear);
  depthAttachment->setStoreAction(MTL::StoreActionDontCare);
  depthAttachment->setClearDepth(1.0);
}
