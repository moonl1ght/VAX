//
// Created by Alexander Lakhonin on 26.03.2025.
//

#ifndef BaseModel_hpp
#define BaseModel_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

#include "Mesh.hpp"
#include "PipelineStateManager.hpp"

class BaseModel {
public:
  Transform transform;
  PipelineStateManager::RenderPipelineStateType renderPipelineStateType = PipelineStateManager::RenderPipelineStateType::base;

  BaseModel(std::vector<Mesh*> meshes)
  :_meshes(meshes) { };
  virtual ~BaseModel();

  BaseModel(const BaseModel& rhs) = delete;
  BaseModel(BaseModel&& rhs);

  BaseModel & operator=(BaseModel & rhs) = delete;
  BaseModel & operator=(BaseModel && rhs);

  std::vector<Mesh*>& meshes() const noexcept;
  virtual void draw(MTL::RenderCommandEncoder* const renderCommandEncoder, MTL::RenderPipelineState* renderPipelineState) const;

protected:
  std::vector<Mesh*> _meshes;
};

#endif /* BaseModel_hpp */
