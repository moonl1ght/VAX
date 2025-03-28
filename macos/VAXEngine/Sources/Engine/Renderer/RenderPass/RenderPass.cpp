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
