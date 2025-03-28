//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "RenderPipelineStateManager.hpp"

RenderPipelineStateManager::~RenderPipelineStateManager() {
  renderPipelineState->release();
  
}
