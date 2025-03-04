//
// Created by Alexander Lakhonin on 05.02.2025.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>
#include <Metal/Metal.hpp>

#include "MeshBuffer.hpp"
#include "Transform.hpp"

class Mesh {
public:
  Transform transform;

  explicit Mesh(std::vector<MeshBuffer> vertexBuffers, u_int vertexCount);
  virtual ~Mesh();

  void draw(MTL::RenderCommandEncoder * const renderCommandEncoder) const;

private:
  u_int _vertexCount;
  std::vector<MeshBuffer> _vertexBuffers;

  void drawPrimitives(MTL::RenderCommandEncoder * const renderCommandEncoder) const;
};

#endif /* Mesh_hpp */
