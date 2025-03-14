//
// Created by Alexander Lakhonin on 17.02.2025.
//

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include "Mesh.hpp"

class Model {
public:
  Model(std::vector<Mesh*> meshes): _meshes(meshes) { };
  virtual ~Model();

  Model(const Model& rhs);
  Model(Model&& rhs);

  Model & operator=(Model & rhs) = delete;
  Model & operator=(Model && rhs);

  std::vector<Mesh*>& meshes() const noexcept;

private:
  std::vector<Mesh*> _meshes;
};

#endif /* Model_hpp */
