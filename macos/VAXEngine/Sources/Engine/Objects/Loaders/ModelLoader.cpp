//
// Created by Alexander Lakhonin on 13.03.2025.
//

#include "ModelLoader.hpp"
#include <assimp/postprocess.h>
#include <iostream>

Model ModelLoader::loadModel(std::string filePath) {
  Assimp::Importer assimpImporter;
  const aiScene* scene = assimpImporter.ReadFile((bundleResourcesPath + filePath).c_str(),
                                                 aiProcess_Triangulate |
                                                 aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "Error: " << assimpImporter.GetErrorString() << std::endl;
    throw std::exception();
  }
  std::cout << "scene loaded" << std::endl;
  auto textureInfo = loadTextures(scene);
  std::vector<Mesh*> meshes = processNode(scene->mRootNode, scene, std::get<0>(textureInfo));
  return Model(meshes, std::get<1>(textureInfo));
}

std::vector<Mesh*> ModelLoader::processNode(aiNode* node,
                                            const aiScene* scene,
                                            std::unordered_map<std::string, int> textureIndexMap) {
  //  std::cout << "Processing node: meshes " << node->mNumMeshes << " children: " << node->mNumChildren << std::endl;
  std::vector<Mesh*> meshes = std::vector<Mesh*>();
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(aiMesh, scene, textureIndexMap));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    std::vector<Mesh*> nodeMeshes = processNode(node->mChildren[i], scene, textureIndexMap);
    meshes.insert(meshes.end(), nodeMeshes.begin(), nodeMeshes.end());
  }
  return meshes;
}

Mesh* ModelLoader::processMesh(aiMesh* aiMesh,
                               const aiScene* scene,
                               std::unordered_map<std::string, int> textureIndexMap) {
  std::vector<Mesh::Vertex> vertices;
  std::vector<uint32_t> indices;
  std::unordered_map<Mesh::Vertex, uint32_t> uniqueVertices;
//
//  // Get texture file names
  aiString diffuseTextureName, specularTextureName, normalTextureName, emissiveTextureName;
//  aiMesh.nNum
//  std::cout << "Mat index: " << aiMesh->mMaterialIndex << std::endl;
  aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
  material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTextureName);
//  material->GetTexture(aiTextureType_SPECULAR, 0, &specularTextureName);
//  material->GetTexture(aiTextureType_HEIGHT, 0, &normalTextureName);
//  material->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTextureName);
//
  // Extract Per-Vertex Data
  for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
    aiFace face = aiMesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      unsigned int vertexIndex = face.mIndices[j];
      aiVector3D pos = aiMesh->mVertices[vertexIndex];
      aiVector3D normal = aiMesh->mNormals[vertexIndex];
      aiVector3D texCoord = aiMesh->mTextureCoords[0][vertexIndex];
//      aiVector3D tangent = aiMesh->mTangents[vertexIndex];
//      aiVector3D bitangent = aiMesh->mTangents[vertexIndex];

      Mesh::Vertex vertex;
      vertex.position = {pos.x, pos.y, pos.z};
      vertex.uv = {texCoord.x, texCoord.y};
      vertex.color = {1, 0, 0};
      vertex.normal = {normal.x, normal.y, normal.z};
//      vertex.tangent = {tangent.x, tangent.y, tangent.z};
//      vertex.bitangent = {bitangent.x, bitangent.y, bitangent.z};
//      vertex.diffuseTextureIndex = {textureIndexMap[diffuseTextureName.C_Str()]};
//      vertex.specularTextureIndex = {textureIndexMap[specularTextureName.C_Str()]};
//      vertex.normalMapIndex = {textureIndexMap[normalTextureName.C_Str()]};
//      vertex.emissiveMapIndex = {textureIndexMap[emissiveTextureName.C_Str()]};
      // Check if the vertex is unique or not
      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      // Add the index of the vertex to the indices vector
      indices.push_back(uniqueVertices[vertex]);
    }
  }
  static unsigned long allvertexCount = 0;
  unsigned long vertexCount = vertices.size();
  allvertexCount = allvertexCount + vertexCount;
  std::cout << "Processing mesh: vertexCount " << allvertexCount << std::endl;
  unsigned long vertexBufferSize = sizeof(Mesh::Vertex) * vertexCount;
  MTL::Buffer* verticesBuffer = _mtlStack->device().newBuffer(vertices.data(), vertexBufferSize, MTL::ResourceStorageModeShared);
  unsigned long indexCount = indices.size();
  unsigned long indexBufferSize = sizeof(uint32_t) * indexCount;
  MTL::Buffer* indicesBuffer = _mtlStack->device().newBuffer(indices.data(), indexBufferSize, MTL::ResourceStorageModeShared);
  
  Mesh* mesh = new Mesh(MeshBuffer(verticesBuffer), (u_int)vertexCount, indicesBuffer, indexCount);
  mesh->textureIndices.diffuseTextureIndex = textureIndexMap[diffuseTextureName.C_Str()];

  return mesh;
}

std::tuple<std::unordered_map<std::string, int>, Model::Textures*> ModelLoader::loadTextures(const aiScene* scene) {
  std::cout << "Loading Textures..." << std::endl;
  std::vector<std::string> textureFilePaths;
  std::unordered_map<std::string, int> textureIndexMap;
  int textureIndex = 0;
  for (int i = 0;  i < scene->mNumMaterials; i++) {
    aiMaterial* material = scene->mMaterials[i];
    mapTextureIndices(textureFilePaths, aiTextureType_DIFFUSE, material, textureIndex, textureIndexMap);
//    mapTextureIndices(textureFilePaths, aiTextureType_SPECULAR, material, textureIndex);
//    mapTextureIndices(textureFilePaths, aiTextureType_HEIGHT, material, textureIndex);
//    mapTextureIndices(textureFilePaths, aiTextureType_EMISSIVE, material, textureIndex);
  }
  if (textureFilePaths.size() == 0) {
    std::cerr << "Texture Files not found..." << std::endl;
    exit(1);
  }
  TexturesLoader textureLoader = TexturesLoader(_mtlStack);
  Model::Textures* textures = textureLoader.loadTextures(textureFilePaths);
//  textures = new TextureArray(textureFilePaths,
//                              device);
  return {textureIndexMap, textures};
}

void ModelLoader::mapTextureIndices(std::vector<std::string>& textureFilePaths,
                                    aiTextureType textureType,
                                    aiMaterial* material,
                                    int& textureIndex,
                                    std::unordered_map<std::string, int>& textureIndexMap) {
  for (int j = 0; j < material->GetTextureCount(textureType); j++) {
    aiString textureFileName;
    if (material->GetTexture(textureType, 0, &textureFileName) == AI_SUCCESS) {
      if (textureIndexMap[textureFileName.C_Str()] == 0) {
        std::string textureFileNameStr = std::string(textureFileName.C_Str());
//        textureFileNameStr.substr(textureFileNameStr.find_last_of("/\\"), textureFileNameStr.size() + 1);
        std::string textureFilePath = bundleResourcesPath +
        textureFileNameStr.substr(textureFileNameStr.find_last_of("/\\"), textureFileNameStr.size() + 1);
        std::cout << textureIndex+1 << ".) " << textureFilePath << std::endl;
        textureIndexMap[textureFileName.C_Str()] = textureIndex++;
        textureFilePaths.push_back(textureFilePath);
      }
    }
  }
}
