//
// Created by Alexander Lakhonin on 10.02.2025.
//

#include "VertexDescriptor.hpp"
#include "ShaderTypes.h"
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

vax::VertexDescriptor vax::VertexDescriptor::createSimpleVertexDescriptor() {
  MTL::VertexDescriptor* vd = MTL::VertexDescriptor::alloc()->init();

  int offset = 0;
  vd->attributes()->object(kVertexAttributePosition)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributePosition)->setOffset(0);
  vd->attributes()->object(kVertexAttributePosition)->setBufferIndex(0);
  offset += sizeof(simd_float3);
  vd->layouts()->object(0)->setStride(offset);

//  vd->attributes()->object(kVertexAttributeUV)->setFormat(MTL::VertexFormatFloat3);
//  vd->attributes()->object(kVertexAttributeUV)->setOffset(offset);
//  vd->attributes()->object(kVertexAttributeUV)->setBufferIndex(0);
//  offset += sizeof(simd_float3);
//  vd->layouts()->object(0)->setStride(offset);
//  std::cout << sizeof(simd_float3) << " " << sizeof(simd_float4) << std::endl;

  vd->attributes()->object(kVertexAttributeVertexColor)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeVertexColor)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeVertexColor)->setBufferIndex(0);
  offset += sizeof(simd_float3);
  vd->layouts()->object(0)->setStride(offset);

  vd->attributes()->object(kVertexAttributeUV)->setFormat(MTL::VertexFormatFloat3);
  vd->attributes()->object(kVertexAttributeUV)->setOffset(offset);
  vd->attributes()->object(kVertexAttributeUV)->setBufferIndex(0);
  offset += sizeof(simd_float3);
  vd->layouts()->object(0)->setStride(offset);

//  vd->attributes()->object(kVertexAttributeUV)->setFormat(MTL::VertexFormatFloat2);
//  vd->attributes()->object(kVertexAttributeUV)->setOffset(offset);
//  vd->attributes()->object(kVertexAttributeUV)->setBufferIndex(0);
//  offset += sizeof(simd_float2);
//  std::cout << sizeof(simd_float3) << " " << sizeof(simd_float4) << std::endl;


//  vd->layouts()->object(0)->setStride(offset);

  return vax::VertexDescriptor(vd);
}
