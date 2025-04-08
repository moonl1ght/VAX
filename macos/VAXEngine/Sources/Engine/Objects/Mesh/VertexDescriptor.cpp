//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include "VertexDescriptor.hpp"
#include "ShadersCommon.h"

#include <iostream>

vax::VertexDescriptor::VertexDescriptor(MTL::VertexDescriptor* mtlVertexDescriptor)
: _mtlVertexDescriptor(mtlVertexDescriptor)
{ }

vax::VertexDescriptor::~VertexDescriptor() {
  _mtlVertexDescriptor->release();
}

const MTL::VertexDescriptor& vax::VertexDescriptor::vertexDescriptor() {
  return *_mtlVertexDescriptor;
}

vax::VertexDescriptor vax::VertexDescriptor::createBaseVertexDescriptor() {
  MTL::VertexDescriptor* vd = MTL::VertexDescriptor::alloc()->init();

  int offset = 0;
  vd->attributes()->object(kVertexAttributePosition)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributePosition)->setOffset(0);
  vd->attributes()->object(kVertexAttributePosition)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  vd->attributes()->object(kVertexAttributeNormal)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeNormal)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeNormal)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  vd->attributes()->object(kVertexAttributeVertexColor)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeVertexColor)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeVertexColor)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  vd->attributes()->object(kVertexAttributeTangent)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeTangent)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeTangent)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  vd->attributes()->object(kVertexAttributeBitangent)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeBitangent)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeBitangent)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  vd->attributes()->object(kVertexAttributeUV)->setFormat(MTL::VertexFormatFloat2);
  vd->attributes()->object(kVertexAttributeUV)->setOffset(0);
  vd->attributes()->object(kVertexAttributeUV)->setBufferIndex(kUVBufferIndex);
  vd->layouts()->object(kUVBufferIndex)->setStride(sizeof(simd_float2));

  return vax::VertexDescriptor(vd);
}

vax::VertexDescriptor vax::VertexDescriptor::createPrimitiveVertexDescriptor() {
  MTL::VertexDescriptor* vd = MTL::VertexDescriptor::alloc()->init();

  int offset = 0;
  vd->attributes()->object(kVertexAttributePosition)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributePosition)->setOffset(0);
  vd->attributes()->object(kVertexAttributePosition)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  vd->attributes()->object(kVertexAttributeVertexColor)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeVertexColor)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeVertexColor)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  return vax::VertexDescriptor(vd);
}

vax::VertexDescriptor vax::VertexDescriptor::createMeshVertexDescriptor() {
  MTL::VertexDescriptor* vd = MTL::VertexDescriptor::alloc()->init();

  int offset = 0;
  vd->attributes()->object(kVertexAttributePosition)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributePosition)->setOffset(0);
  vd->attributes()->object(kVertexAttributePosition)->setBufferIndex(kVertexBufferIndex);
  offset += sizeof(simd_float3);
  vd->layouts()->object(kVertexBufferIndex)->setStride(offset);

  return vax::VertexDescriptor(vd);
}
