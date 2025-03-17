//
// Created by Alexander Lakhonin on 05.02.2025.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>
#include <Metal/Metal.hpp>

#include "MeshBuffer.hpp"
#include "Transform.hpp"
#include "simd_utils.h"
#include "ShaderTypes.h"

class Mesh {
public:

  enum class DrawingMode {
    primitives, indexedPrimitives
  };

  struct Vertex {
    simd_float3 position;
    simd_float3 color;
    simd_float2 uv;
  };

  Transform transform;
  DrawingMode drawingMode;
  TextureIndices textureIndices;

  explicit Mesh(MeshBuffer vertexBuffer, u_int vertexCount, DrawingMode drawingMode = DrawingMode::primitives);
  Mesh(MeshBuffer vertexBuffer,
       u_int vertexCount,
       MTL::Buffer* indicesBuffer,
       unsigned long indexCount,
       DrawingMode drawingMode = DrawingMode::indexedPrimitives);
  Mesh(const Mesh & rhs);
  Mesh(Mesh && rhs) = delete;
  virtual ~Mesh();

  Mesh& operator=(Mesh & rhs);
  Mesh& operator=(Mesh && rhs) = delete;

  void draw(MTL::RenderCommandEncoder * const renderCommandEncoder) const;

private:
  u_int _vertexCount;
  MeshBuffer _vertexBuffer;
  MTL::Buffer* _indicesBuffer;
  unsigned long _indexCount;

  void drawPrimitives(MTL::RenderCommandEncoder * const renderCommandEncoder) const;
  void drawIndexedPrimitives(MTL::RenderCommandEncoder * const renderCommandEncoder) const;
};

inline bool operator==(const Mesh::Vertex& lhs, const Mesh::Vertex& rhs) {
  return lhs.position.x == rhs.position.x &&
  lhs.position.y == rhs.position.y &&
  lhs.position.z == rhs.position.z &&
  //  lhs.normal.x == rhs.normal.x &&
  //  lhs.normal.y == rhs.normal.y &&
  //  lhs.normal.z == rhs.normal.z &&
  lhs.uv.x == rhs.uv.x &&
  lhs.uv.y == rhs.uv.y;
  //  lhs.diffuseTextureIndex == rhs.diffuseTextureIndex;
}

namespace std {
  template<> struct hash<Mesh::Vertex> {
    size_t operator()(Mesh::Vertex const& vertex) const {
      size_t h1 = hash<simd_float3>{}(vertex.position);
      //      size_t h2 = hash<float3>{}(vertex.normal);
      size_t h3 = hash<simd_float2>{}(vertex.uv);
      //      size_t h4 = hash<int>{}(vertex.diffuseTextureIndex);

      return h1 ^ (h3 << 1);
    }
  };
}

#endif /* Mesh_hpp */
