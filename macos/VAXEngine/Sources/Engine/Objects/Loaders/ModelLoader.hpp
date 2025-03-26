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
#include "TexturesLoader.hpp"

class ModelLoader
{
public:
  std::string bundleResourcesPath;

  explicit ModelLoader(MTLStack *mtlStack) : _mtlStack(mtlStack) {};
  virtual ~ModelLoader() {};

  Model loadModel(std::string filePath);

private:
  MTLStack *_mtlStack;

  std::vector<Mesh *> processNode(
    aiNode *node,
    const aiScene *scene,
    std::unordered_map<std::string, int> textureIndexMap);
  Mesh *processMesh(
    aiMesh *aiMesh,
    const aiScene *scene,
    std::unordered_map<std::string, int> textureIndexMap);
  std::tuple<std::unordered_map<std::string, int>, Model::Textures *> loadTextures(const aiScene *scene);
  void mapTextureIndices(
    std::vector<std::string> &textureFilePaths,
    aiTextureType textureType,
    aiMaterial *material,
    int &textureIndex,
    std::unordered_map<std::string, int> &textureIndexMap);
};

#endif /* ModelLoader_hpp */
