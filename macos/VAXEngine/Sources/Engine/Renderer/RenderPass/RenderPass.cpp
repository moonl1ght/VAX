//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "RenderPass.hpp"

RenderPass::~RenderPass() {
  _descriptor->release();
}

const MTL::RenderPassDescriptor& RenderPass::descriptor() const noexcept {
  return *_descriptor;
}

void RenderPass::updateRenderPassDescriptor(CA::MetalDrawable* drawable) noexcept { }

void RenderPass::updateRenderPassDescriptor() noexcept { }

void RenderPass::resize(const vax::Size viewSize, const vax::Size drawableSize) noexcept { }
