//
// Created by Alexander Lakhonin on 31.03.2025.
//

#include "ShadowRenderPass.hpp"

ShadowRenderPass::~ShadowRenderPass() { }

void ShadowRenderPass::draw(MTL::CommandBuffer *commandBuffer, Scene *scene, PipelineStateManager *pipelineStateManager) const noexcept {

}

void ShadowRenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept { }
