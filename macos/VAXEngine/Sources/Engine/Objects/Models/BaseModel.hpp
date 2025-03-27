//
// Created by Alexander Lakhonin on 26.03.2025.
//

#ifndef BaseModel_hpp
#define BaseModel_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

#include "Mesh.hpp"

class BaseModel {
public:
  BaseModel(std::vector<Mesh*> meshes)
  :_meshes(meshes) { };
  virtual ~BaseModel();

  BaseModel(const BaseModel& rhs) = delete;
  BaseModel(BaseModel&& rhs);

  BaseModel & operator=(BaseModel & rhs) = delete;
  BaseModel & operator=(BaseModel && rhs);

  std::vector<Mesh*>& meshes() const noexcept;
  virtual void draw(MTL::RenderCommandEncoder* const renderCommandEncoder, MTL::RenderPipelineState* renderPipelineState) const = 0;

protected:
  std::vector<Mesh*> _meshes;
};

#endif /* BaseModel_hpp */
