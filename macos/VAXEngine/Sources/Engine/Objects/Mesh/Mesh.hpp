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
    struct GPUVertex {
      simd_float3 position;
      simd_float3 normal;
      simd_float3 color;
      simd_float3 tangent;
      simd_float3 bitangent;
    };
    struct UV {
      simd_float2 uv;
    };

    simd_float3 position;
    simd_float3 normal;
    simd_float3 color;
    simd_float3 tangent;
    simd_float3 bitangent;
    simd_float2 uv;

    GPUVertex getGPUVertex() const {
      struct GPUVertex gpuVertex {
        .position = position,
        .normal = normal,
        .color = color,
        .tangent = tangent,
        .bitangent = bitangent
      };
      return gpuVertex;
    }

    UV getUV() const {
      struct UV gpuuv {
        .uv = uv
      };
      return gpuuv;
    }
  };

  Transform transform;
  DrawingMode drawingMode;
  TextureIndices textureIndices;

  explicit Mesh(MeshBuffer vertexBuffer, u_int vertexCount, DrawingMode drawingMode = DrawingMode::primitives);
  Mesh(MeshBuffer vertexBuffer,
       MTL::Buffer* uvBuffer,
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
  MTL::Buffer* _uvBuffer;
  MTL::Buffer* _indicesBuffer;
  unsigned long _indexCount;

  void drawPrimitives(MTL::RenderCommandEncoder * const renderCommandEncoder) const;
  void drawIndexedPrimitives(MTL::RenderCommandEncoder * const renderCommandEncoder) const;
};

inline bool operator==(const Mesh::Vertex& lhs, const Mesh::Vertex& rhs) {
  return lhs.position.x == rhs.position.x && lhs.position.y == rhs.position.y && lhs.position.z == rhs.position.z &&
  lhs.normal.x == rhs.normal.x && lhs.normal.y == rhs.normal.y && lhs.normal.z == rhs.normal.z &&
  lhs.color.x == rhs.color.x && lhs.color.y == rhs.color.y && lhs.color.z == rhs.color.z &&
  lhs.uv.x == rhs.uv.x && lhs.uv.y == rhs.uv.y &&
  lhs.tangent.x == rhs.tangent.x && lhs.tangent.y == rhs.tangent.y && lhs.tangent.z == rhs.tangent.z &&
  lhs.bitangent.x == rhs.bitangent.x && lhs.bitangent.y == rhs.bitangent.y && lhs.bitangent.z == rhs.bitangent.z;
}

namespace std {
  template<> struct hash<Mesh::Vertex> {
    size_t operator()(Mesh::Vertex const& vertex) const {
      size_t h1 = hash<simd_float3>{}(vertex.position);
      size_t h2 = hash<simd_float3>{}(vertex.normal);
      size_t h3 = hash<simd_float3>{}(vertex.color);
      size_t h4 = hash<simd_float2>{}(vertex.uv);
      size_t h5 = hash<simd_float3>{}(vertex.tangent);
      size_t h6 = hash<simd_float3>{}(vertex.bitangent);

      return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5);
    }
  };
}

#endif /* Mesh_hpp */
