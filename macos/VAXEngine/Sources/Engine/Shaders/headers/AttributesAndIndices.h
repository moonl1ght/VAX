//
// Created by Alexander Lakhonin on 10.02.2025.
//

#ifndef AttributesAndIndices_h
#define AttributesAndIndices_h

typedef enum VertexAttributes {
  kVertexAttributePosition = 0,
  kVertexAttributeNormal = 1,
  kVertexAttributeVertexColor = 2,
  kVertexAttributeUV = 3,
  kVertexAttributeTangent = 4,
  kVertexAttributeBitangent = 5
} VertexAttributes;

typedef enum {
  kVertexBufferIndex = 0,
  kUVBufferIndex = 1,
  kVertexUniformsBufferIndex = 10,
  kModelUniformsBufferIndex = 11,
  kShadowUniformsBufferIndex = 12
} VertexBufferIndices;

typedef enum {
  kFragmentUniformsIndex = 3,
  kLightIndex = 4,
  kTextureIndicesIndex = 5,
  kTextureInfoIndex = 6
} FragmentBufferIndices;

typedef enum {
  kTextureArrayIndex = 0,
  kTextureShadowIndex = 1
} FragmentTexturesIndices;

#endif /* AttributesAndIndices_h */
