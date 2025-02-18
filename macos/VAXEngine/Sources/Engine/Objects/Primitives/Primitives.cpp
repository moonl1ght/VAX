//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "Primitives.hpp"
#include <simd/simd.h>

using namespace MTL;

Model Primitives::createRGBTriangle(Device &device) {
  simd_float3 vertices[] = {
    { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f },
    { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }
  };
  Buffer* buffer = device.newBuffer(&vertices, sizeof(vertices), ResourceStorageModeShared);
  Mesh* mesh = new Mesh(std::vector<MeshBuffer>({ MeshBuffer(buffer) }), 3);
  return Model(mesh);
}
