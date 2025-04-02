//
// Created by Alexander Lakhonin on 26.03.2025.
//

#include "PrimitiveModel.hpp"

void PrimitiveModel::draw(
  MTL::RenderCommandEncoder *const renderCommandEncoder,
  MTL::RenderPipelineState* renderPipelineState,
  Mesh::RenderingMode renderingMode
) const {
  if (renderPipelineState) {
    renderCommandEncoder->setRenderPipelineState(renderPipelineState);
  }
  BaseModel::draw(renderCommandEncoder, renderPipelineState);
  for (auto mesh: _meshes) {
    mesh->draw(renderCommandEncoder, renderingMode);
  }
}
