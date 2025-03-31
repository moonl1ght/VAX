//
// Created by Alexander Lakhonin on 05.02.2025.
//

#include "Primitives.hpp"
#include <simd/simd.h>

using namespace MTL;

std::unique_ptr<PrimitiveModel> Primitives::createRGBTriangle(Device &device) {
  simd_float3 vertices[] = {
    { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f },
    { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }
  };
  Buffer* buffer = device.newBuffer(&vertices, sizeof(vertices), ResourceStorageModeShared);
  Mesh* mesh = new Mesh(MeshBuffer(buffer), 3);
  std::unique_ptr<PrimitiveModel> model = std::make_unique<PrimitiveModel>(PrimitiveModel({mesh}));
  model->renderPipelineStateType = PipelineStateManager::RenderPipelineStateType::base;
  return model;
}

std::unique_ptr<PrimitiveModel> Primitives::createRGBCube(Device &device) {
  simd_float3 A = { -1.0f, 1.0f, 1.0f };
  simd_float3 AC = { 1.0f, 0.0f, 0.0f };
  simd_float3 B = { -1.0f, -1.0f, 1.0f };
  simd_float3 BC = { 0.0f, 1.0f, 0.0f };
  simd_float3 C = { 1.0f, -1.0f, 1.0f };
  simd_float3 CC = { 0.0f, 0.0f, 1.0f };
  simd_float3 D = { 1.0f, 1.0f, 1.0f };
  simd_float3 DC = { 1.0f, 0.6f, 0.4f };

  simd_float3 Q = { -1.0f, 1.0f, -1.0f };
  simd_float3 QC = { 1.0f, 0.0f, 0.0f };
  simd_float3 R = { 1.0f, 1.0f, -1.0f };
  simd_float3 RC = { 0.0f, 1.0f, 0.0f };
  simd_float3 S = { -1.0f, -1.0f, -1.0f };
  simd_float3 SC = { 0.0f, 0.0f, 1.0f };
  simd_float3 T = { 1.0f, -1.0f, -1.0f };
  simd_float3 TC = { 1.0f, 0.6f, 0.4f };

  simd_float3 vertices[] = {
    A,AC,B,BC,C,CC ,A,AC,C,CC,D,DC, //Front
    R,RC,T,TC,S,SC ,Q,QC,R,RC,S,SC, //Back

    Q,QC,S,SC,B,BC ,Q,QC,B,BC,A,AC, //Left
    D,DC,C,CC,T,TC ,D,DC,T,TC,R,RC, //Right

    Q,QC,A,AC,D,DC ,Q,QC,D,DC,R,RC, //Top
    B,BC,S,SC,T,TC ,B,BC,T,TC,C,CC
  };

  Buffer* buffer = device.newBuffer(&vertices, sizeof(vertices), ResourceStorageModeShared);
  Mesh* mesh = new Mesh(MeshBuffer(buffer), 36);
  std::unique_ptr<PrimitiveModel> model = std::make_unique<PrimitiveModel>(PrimitiveModel({mesh}));
  model->renderPipelineStateType = PipelineStateManager::RenderPipelineStateType::base;
  return model;
}

std::unique_ptr<PrimitiveModel> Primitives::createGizmo(Device &device) {
  simd_float3 vertices[] = {
    { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }
  };
  Buffer* buffer = device.newBuffer(&vertices, sizeof(vertices), ResourceStorageModeShared);
  Mesh* mesh = new Mesh(MeshBuffer(buffer), 6, Mesh::DrawingMode::primitives);
  mesh->primitiveType = MTL::PrimitiveTypeLine;
  std::unique_ptr<PrimitiveModel> model = std::make_unique<PrimitiveModel>(PrimitiveModel({mesh}));
  model->renderPipelineStateType = PipelineStateManager::RenderPipelineStateType::primitive;
  return model;
}

std::unique_ptr<PrimitiveModel> Primitives::plane(Device &device) {
  simd_float3 vertices[] = {
    { 1.0f, 0.0f, -1.0f }, { 0.5f, 0.8f, 1.0f },
    { -1.0f, 0.0f, 1.0f }, { 0.5f, 0.8f, 1.0f },
    { 1.0f, 0.0f, 1.0f }, { 0.5f, 0.8f, 1.0f },
    { 1.0f, 0.0f, -1.0f }, { 0.5f, 0.8f, 1.0f },
    { -1.0f, 0.0f, 1.0f }, { 0.5f, 0.8f, 1.0f },
    { -1.0f, 0.0f, -1.0f }, { 0.5f, 0.8f, 1.0f }
  };
  Buffer* buffer = device.newBuffer(&vertices, sizeof(vertices), ResourceStorageModeShared);
  Mesh* mesh = new Mesh(MeshBuffer(buffer), 6, Mesh::DrawingMode::primitives);
  std::unique_ptr<PrimitiveModel> model = std::make_unique<PrimitiveModel>(PrimitiveModel({mesh}));
  model->renderPipelineStateType = PipelineStateManager::RenderPipelineStateType::primitive;
  return model;
}
