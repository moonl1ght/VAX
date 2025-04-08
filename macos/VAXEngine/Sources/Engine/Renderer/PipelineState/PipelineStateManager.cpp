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
  shadowPipelineState->release();
  shadowPipelineState = nullptr;
  gBufferPipelineState->release();
  gBufferPipelineState = nullptr;
  sunlightPipelineState->release();
  sunlightPipelineState = nullptr;
}

MTL::RenderPipelineState* PipelineStateManager::renderPipelineStateForType(RenderPipelineStateType type, bool gBufferRendering) const noexcept {
  if (gBufferRendering) {
    return gBufferPipelineState;
  } else {
    switch (type) {
      case RenderPipelineStateType::base:
        return renderPipelineState;
      case RenderPipelineStateType::primitive:
        return primitivePipelineState;
      case RenderPipelineStateType::gizmo:
        return gizmoPipelineState;
    }
  }
}
