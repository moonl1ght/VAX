//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "MeshBuffer.hpp"
#include <iostream>

using namespace MTL;

MeshBuffer::MeshBuffer(MTL::Buffer * buffer)
: _buffer(buffer)
, _offset(0)
{ }

MeshBuffer::MeshBuffer(MTL::Buffer * buffer, uint offset)
: _buffer(buffer)
, _offset(offset)
{ }

MeshBuffer::~MeshBuffer() {
  std::cout << "delete mesh Buffer" << std::endl;
  _buffer->release();
}

MeshBuffer::MeshBuffer(const MeshBuffer& other)
: _buffer(other._buffer->retain())
, _offset(other._offset) {
}

MeshBuffer& MeshBuffer::operator=(MeshBuffer & rhs) {
  return *this;
}

const Buffer& MeshBuffer::buffer() const {
  return *_buffer;
}

uint MeshBuffer::offset() const {
  return _offset;
}
