//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "PipelineStateManager.hpp"

PipelineStateManager::~PipelineStateManager() {
  renderPipelineState->release();
  renderPipelineState = nullptr;
  gizmoPipelineState->release();
  gizmoPipelineState = nullptr;
  depthStencilState->release();
  depthStencilState = nullptr;
  primitivePipelineState->release();
  primitivePipelineState = nullptr;
}

MTL::RenderPipelineState* PipelineStateManager::renderPipelineStateForType(RenderPipelineStateType type) const noexcept {
  switch (type) {
    case RenderPipelineStateType::base:
    return renderPipelineState;
    case RenderPipelineStateType::primitive:
      return primitivePipelineState;
    case RenderPipelineStateType::gizmo:
      return gizmoPipelineState;
  }
}
