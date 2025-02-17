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
  MeshBuffer(MTL::Buffer * buffer, uint offset);
  MeshBuffer(const MeshBuffer & rhs);
  MeshBuffer(MeshBuffer && rhs) = default;
  virtual ~MeshBuffer();

  MeshBuffer & operator=(MeshBuffer & rhs);
  MeshBuffer & operator=(MeshBuffer && rhs) = default;

  const MTL::Buffer & buffer() const;
  uint offset() const;

private:
  MTL::Buffer* _buffer;
  uint _offset;
};

#endif /* MeshBuffer_hpp */
