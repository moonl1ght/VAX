//
// Created by Alexander Lakhonin on 14.03.2025.
//

#ifndef TexturesLoader_hpp
#define TexturesLoader_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

#include "stb_image.h"

#include "Model.hpp"
#include "MTLStack.hpp"

class TexturesLoader {
public:
  explicit TexturesLoader(MTLStack* mtlStack): _mtlStack(mtlStack) { };
  virtual ~TexturesLoader() { };

  Model::Textures* loadTextures(std::vector<std::string>& filePaths);

private:
  MTLStack* _mtlStack;

//  std::vector<Mesh*> processNode(aiNode* node, const aiScene* scene);
//  Mesh* processMesh(aiMesh* aiMesh, const aiScene* scene);
};

#endif /* TexturesLoader_hpp */
