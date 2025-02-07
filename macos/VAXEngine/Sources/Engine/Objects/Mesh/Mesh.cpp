//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "Mesh.hpp"

Mesh::Mesh(std::vector<MeshBuffer> vertexBuffers, u_int vertexCount)
: _vertexBuffers(vertexBuffers)
, _vertexCount(vertexCount)
{ };

Mesh::~Mesh() { };
