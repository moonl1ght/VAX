//
// Created by Alexander Lakhonin on 13.03.2025.
//

#ifndef ModelLoader_hpp
#define ModelLoader_hpp

#include <stdio.h>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <Metal/Metal.hpp>
#include "Mesh.hpp"
#include "Model.hpp"
#include "MTLStack.hpp"

class ModelLoader {
public:
  explicit ModelLoader(MTLStack* mtlStack): _mtlStack(mtlStack) { };
  virtual ~ModelLoader() { };

  Model loadModel(std::string filePath);

private:
  MTLStack* _mtlStack;

  std::vector<Mesh*> processNode(aiNode* node, const aiScene* scene);
  Mesh* processMesh(aiMesh* aiMesh, const aiScene* scene);
};

#endif /* ModelLoader_hpp */
