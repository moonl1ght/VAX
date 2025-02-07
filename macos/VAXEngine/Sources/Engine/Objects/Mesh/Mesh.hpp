//
// Created by Alexander Lakhonin on 05.02.2025.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>
#include <Metal/Metal.hpp>

#include "MeshBuffer.hpp"

class Mesh {
public:
  explicit Mesh(std::vector<MeshBuffer> vertexBuffers, u_int vertexCount);
  virtual ~Mesh();

private:
  u_int _vertexCount;
  std::vector<MeshBuffer> _vertexBuffers;
};

#endif /* Mesh_hpp */
