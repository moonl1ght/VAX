//
// Created by Alexander Lakhonin on 17.02.2025.
//

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include "Mesh.hpp"

class Model {
public:
  Model(Mesh* mesh): _mesh(mesh) { };
  virtual ~Model();

  Model(const Model& rhs) = delete;
  Model(Model&& rhs);

  Model & operator=(Model & rhs) = delete;
  Model & operator=(Model && rhs);

  Mesh& mesh() const noexcept;

private:
  Mesh* _mesh;
};

#endif /* Model_hpp */
