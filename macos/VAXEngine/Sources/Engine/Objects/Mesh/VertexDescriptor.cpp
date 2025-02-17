//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include "VertexDescriptor.hpp"
#include "ShaderTypes.h"

VAX::VertexDescriptor::VertexDescriptor(MTL::VertexDescriptor* mtlVertexDescriptor)
: _mtlVertexDescriptor(mtlVertexDescriptor)
{ }

VAX::VertexDescriptor::~VertexDescriptor() {
  _mtlVertexDescriptor->release();
}

const MTL::VertexDescriptor& VAX::VertexDescriptor::vertexDescriptor() {
  return *_mtlVertexDescriptor;
}

VAX::VertexDescriptor VAX::VertexDescriptor::createSimpleVertexDescriptor() {
  MTL::VertexDescriptor* vd = MTL::VertexDescriptor::alloc()->init();

  int offset = 0;
  vd->attributes()->object(kVertexAttributePosition)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributePosition)->setOffset(0);
  vd->attributes()->object(kVertexAttributePosition)->setBufferIndex(0);
  offset += sizeof(simd_float3);

  vd->attributes()->object(kVertexAttributeVertexColor)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeVertexColor)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeVertexColor)->setBufferIndex(0);
  offset += sizeof(simd_float3);

  vd->layouts()->object(0)->setStride(offset);

  return VAX::VertexDescriptor(vd);
}
