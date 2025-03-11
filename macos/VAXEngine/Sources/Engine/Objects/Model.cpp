//
// Created by Alexander Lakhonin on 17.02.2025.
//

#include "Model.hpp"
#include <iostream>
#include <assimp/Importer.hpp>

using namespace std;

Model::~Model() {
  delete _mesh;
  cout << "delete model" << endl;
}

Model::Model(Model&& rhs): _mesh(rhs._mesh) {
  rhs._mesh = nullptr;
  cout << "move constructor" << endl;

  Assimp::Importer();
}

Model& Model::operator=(Model&& rhs)
{
  cout << "move assignment" << endl;
  if (this != &rhs) {
    delete _mesh;
    _mesh = rhs._mesh;
    rhs._mesh = nullptr;
  }
  return *this;
}

Mesh& Model::mesh() const noexcept {
  return *_mesh;
}
