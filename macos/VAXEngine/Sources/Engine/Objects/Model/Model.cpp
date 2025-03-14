//
// Created by Alexander Lakhonin on 17.02.2025.
//

#include "Model.hpp"
#include <iostream>

using namespace std;

Model::~Model() {
  for (auto mesh : _meshes) {
    delete mesh;
  }
//  cout << "delete model" << endl;
}

Model::Model(const Model& rhs) {
//  cout << "copy constructor model" << endl;
}

Model::Model(Model&& rhs): _meshes(rhs._meshes) {
  rhs._meshes.clear();
//  cout << "move constructor model" << endl;
}

Model& Model::operator=(Model&& rhs)
{
//  cout << "move assignment" << endl;
  if (this != &rhs) {
    for (auto mesh : _meshes) {
      delete mesh;
    }
    _meshes = rhs._meshes;
    rhs._meshes.clear();
  }
  return *this;
}

std::vector<Mesh*>& Model::meshes() const noexcept {
  return const_cast<std::vector<Mesh*>&>(_meshes);
}
