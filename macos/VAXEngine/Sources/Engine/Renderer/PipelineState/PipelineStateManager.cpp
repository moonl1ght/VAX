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
}
