//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "MeshBuffer.hpp"

using namespace MTL;

MeshBuffer::MeshBuffer(MTL::Buffer * buffer)
: _buffer(buffer)
{
}

MeshBuffer::~MeshBuffer() {
  _buffer->release();
}

const Buffer& MeshBuffer::buffer() const
{
  return *_buffer;
}
