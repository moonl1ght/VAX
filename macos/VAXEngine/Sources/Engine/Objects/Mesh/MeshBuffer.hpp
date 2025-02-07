//
// Created by Alexander Lakhonin on 05.02.2025.
//

#ifndef MeshBuffer_hpp
#define MeshBuffer_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

class MeshBuffer {
public:
  explicit MeshBuffer(MTL::Buffer * buffer);
  MeshBuffer(const MeshBuffer & rhs) = default;
  MeshBuffer(MeshBuffer && rhs) = default;
  virtual ~MeshBuffer();

  MeshBuffer & operator=(MeshBuffer & rhs) = default;
  MeshBuffer & operator=(MeshBuffer && rhs) = default;

  const MTL::Buffer & buffer() const;

private:
  MTL::Buffer* _buffer;
};

#endif /* MeshBuffer_hpp */
