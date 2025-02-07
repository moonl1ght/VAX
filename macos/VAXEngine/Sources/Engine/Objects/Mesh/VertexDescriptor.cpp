//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include "VertexDescriptor.hpp"
#include "ShaderTypes.h"

VAX::VertexDescriptor::VertexDescriptor(MTL::VertexDescriptor* mtlVertexDescriptor)
: _mtlVertexDescriptor(mtlVertexDescriptor)
{
}

VAX::VertexDescriptor::~VertexDescriptor() {
  _mtlVertexDescriptor->release();
}

VAX::VertexDescriptor VAX::VertexDescriptor::createSimpleVertexDescriptor() {
  MTL::VertexDescriptor* vd = MTL::VertexDescriptor::alloc()->init();
  vd->attributes()->object(kVertexAttributePosition)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributePosition)->setOffset(0);
  vd->attributes()->object(kVertexAttributePosition)->setBufferIndex(0);

  return VAX::VertexDescriptor(vd);
}
