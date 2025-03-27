//
// Created by Alexander Lakhonin on 26.03.2025.
//

#include "BaseModel.hpp"

BaseModel::~BaseModel() {
  for (auto mesh : _meshes) {
    delete mesh;
  }
}

BaseModel::BaseModel(BaseModel&& rhs): _meshes(rhs._meshes) {
  rhs._meshes.clear();
}

BaseModel& BaseModel::operator=(BaseModel&& rhs)
{
  if (this != &rhs) {
    for (auto mesh : _meshes) {
      delete mesh;
    }
    _meshes = rhs._meshes;
    rhs._meshes.clear();
  }
  return *this;
}

std::vector<Mesh*>& BaseModel::meshes() const noexcept {
  return const_cast<std::vector<Mesh*>&>(_meshes);
}
