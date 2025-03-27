//
// Created by Alexander Lakhonin on 17.02.2025.
//

#include "Model.hpp"
#include <iostream>

using namespace std;

Model::~Model() {
  delete _textures;
}

Model::Model(Model&& rhs): BaseModel(rhs._meshes), _textures(rhs._textures) {
  rhs._meshes.clear();
  rhs._textures = nullptr;
}

Model& Model::operator=(Model&& rhs)
{
  if (this != &rhs) {
    _textures = nullptr;
    for (auto mesh : _meshes) {
      delete mesh;
    }
    _textures = rhs._textures;
    _meshes = rhs._meshes;
    rhs._meshes.clear();
  }
  return *this;
}

void Model::draw(MTL::RenderCommandEncoder *const renderCommandEncoder, MTL::RenderPipelineState* renderPipelineState) const {
  if (renderPipelineState) {
    renderCommandEncoder->setRenderPipelineState(renderPipelineState);
  }
  if (_textures) {
    renderCommandEncoder->setFragmentBuffer(_textures->textureInfosBuffer, 0, 6);
    renderCommandEncoder->setFragmentTexture(_textures->textureArray, 0);
  }
  for (auto mesh: _meshes) {
    mesh->draw(renderCommandEncoder);
  }
}

Model::Textures::Textures(MTL::Texture *textureArray,
                          std::vector<TextureInfo> textureInfos,
                          MTL::Buffer* textureInfosBuffer)
: textureArray(textureArray)
, textureInfos(textureInfos)
, textureInfosBuffer(textureInfosBuffer)
{ };

Model::Textures::Textures(const Model::Textures & rhs)
: textureArray(rhs.textureArray->retain())
, textureInfos(rhs.textureInfos)
, textureInfosBuffer(rhs.textureInfosBuffer->retain())
{ };

Model::Textures::~Textures() {
  textureArray->release();
  textureInfosBuffer->release();
}

Model::Textures& Model::Textures::operator=(Mesh & rhs) {
  textureArray->retain();
  textureInfosBuffer->retain();
  return *this;
}
