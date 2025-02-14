//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "Mesh.hpp"
#include "ShaderTypes.h"

using namespace MTL;

Mesh::Mesh(std::vector<MeshBuffer> vertexBuffers, u_int vertexCount)
: _vertexBuffers(vertexBuffers)
, _vertexCount(vertexCount)
{ };

Mesh::~Mesh() { };

void Mesh::draw(RenderCommandEncoder * const renderCommandEncoder) const {
  for (const auto& buffer : _vertexBuffers) {
    renderCommandEncoder->setVertexBuffer(&buffer.buffer(), buffer.offset(), kVertexAttributePosition);
  }
  drawPrimitives(renderCommandEncoder);
}

void Mesh::drawPrimitives(RenderCommandEncoder * const renderCommandEncoder) const {
  renderCommandEncoder->drawPrimitives(PrimitiveTypeTriangle, NS::UInteger(0), _vertexCount);
}
