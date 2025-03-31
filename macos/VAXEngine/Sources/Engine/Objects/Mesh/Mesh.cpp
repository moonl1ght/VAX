//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "Mesh.hpp"
#include "ShaderTypes.h"

using namespace MTL;

Mesh::Mesh(MeshBuffer vertexBuffer, u_int vertexCount, DrawingMode drawingMode)
: _vertexBuffer(vertexBuffer)
, _vertexCount(vertexCount)
{ };

Mesh::Mesh(
  MeshBuffer vertexBuffer,
  MTL::Buffer* uvBuffer,
  u_int vertexCount,
  Buffer* indicesBuffer,
  unsigned long indexCount,
  DrawingMode drawingMode
)
: _vertexBuffer(vertexBuffer)
, _uvBuffer(uvBuffer)
, _vertexCount(vertexCount)
, _indicesBuffer(indicesBuffer)
, _indexCount(indexCount)
, drawingMode(drawingMode)
{ };

Mesh::Mesh(const Mesh & rhs)
: _vertexBuffer(rhs._vertexBuffer)
, _uvBuffer(rhs._uvBuffer->retain())
, _vertexCount(rhs._vertexCount)
, _indicesBuffer(rhs._indicesBuffer->retain())
{ };

Mesh::~Mesh() {
  _uvBuffer->release();
  _indicesBuffer->release();
};

Mesh& Mesh::operator=(Mesh & rhs) {
  _uvBuffer->retain();
  _indicesBuffer->retain();
  return *this;
}

void Mesh::draw(RenderCommandEncoder * const renderCommandEncoder) const {
  renderCommandEncoder->setVertexBuffer(&_vertexBuffer.buffer(), _vertexBuffer.offset(), kVertexBufferIndex);
  if (_uvBuffer) {
    renderCommandEncoder->setVertexBuffer(_uvBuffer, 0, kUVBufferIndex);
  }
  renderCommandEncoder->setFragmentBytes(&textureIndices, sizeof(textureIndices), 5);
  switch (drawingMode) {
    case DrawingMode::primitives:
      drawPrimitives(renderCommandEncoder);
      break;
    case DrawingMode::indexedPrimitives:
      drawIndexedPrimitives(renderCommandEncoder);
      break;
    default:
      break;
  }
}

void Mesh::drawPrimitives(RenderCommandEncoder * const renderCommandEncoder) const {
  renderCommandEncoder->drawPrimitives(primitiveType, NS::UInteger(0), _vertexCount);
}

void Mesh::drawIndexedPrimitives(RenderCommandEncoder * const renderCommandEncoder) const {
  renderCommandEncoder->drawIndexedPrimitives(
    primitiveType,
    _indexCount,
    MTL::IndexTypeUInt32,
    _indicesBuffer,
    0
  );
}
