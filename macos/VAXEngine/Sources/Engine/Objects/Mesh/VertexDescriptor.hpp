//
// Created by Alexander Lakhonin on 10.02.2025.
//

#ifndef VertexDescriptor_hpp
#define VertexDescriptor_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

namespace VAX {
  class VertexDescriptor {
  public:
    virtual ~VertexDescriptor();
    const MTL::VertexDescriptor& vertexDescriptor();

    static VertexDescriptor createSimpleVertexDescriptor();

  private:
    VertexDescriptor(MTL::VertexDescriptor* mtlVertexDescriptor);
    MTL::VertexDescriptor* _mtlVertexDescriptor;
  };
}

#endif /* VertexDescriptor_hpp */
