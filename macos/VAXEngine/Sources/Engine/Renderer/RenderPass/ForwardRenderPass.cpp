//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "ForwardRenderPass.hpp"

using namespace MTL;

void ForwardRenderPass::draw(CommandBuffer *commandBuffer, Scene *scene) const noexcept {
  RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(_descriptor);

//  renderCommandEncoder->setDepthStencilState(_depthStencilState);
//  //  renderCommandEncoder->setRenderPipelineState(_renderPipelineState);
//
//
//  VertexUniforms vertexUniforms = { scene->camera.viewMatrix(), scene->camera.projectionMatrix() };
//  renderCommandEncoder->setVertexBytes(&vertexUniforms, vertexUniforms.size(), kVertexUniformsBufferIndex);
//
//  auto lights = scene->lights();
//  FragmentUniforms fragmentsUniforms = { scene->camera.transform.position, (uint)lights.size() };
//  renderCommandEncoder->setFragmentBytes(&fragmentsUniforms, sizeof(fragmentsUniforms), 3);
//  renderCommandEncoder->setFragmentBytes(lights.data(), sizeof(Light) * lights.size(), 4);
//  for (auto& model: scene->models()) {
//    model->draw(renderCommandEncoder, _renderPipelineState);
//  }
//
//  scene->gizmo().draw(renderCommandEncoder, _gizmoPipelineState);

  renderCommandEncoder->endEncoding();
}
