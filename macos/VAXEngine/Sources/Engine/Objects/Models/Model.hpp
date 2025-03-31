//
// Created by Alexander Lakhonin on 17.02.2025.
//

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>

#include "BaseModel.hpp"

class Model: public BaseModel {
public:
  class Textures {
  public:
    Textures(MTL::Texture* textureArray,
                      std::vector<TextureInfo> textureInfos,
                      MTL::Buffer* textureInfosBuffer);
    Textures(const Textures & rhs);
    Textures(Textures && rhs) = delete;
    virtual ~Textures();

    Textures& operator=(Mesh & rhs);
    Textures& operator=(Mesh && rhs) = delete;

    MTL::Texture* textureArray;
    std::vector<TextureInfo> textureInfos;
    MTL::Buffer* textureInfosBuffer;
  };

  Model(std::vector<Mesh*> meshes, Textures* textures)
  :BaseModel(meshes), _textures(textures) { };
  ~Model();

  Model(const Model& rhs) = delete;
  Model(Model&& rhs);

  Model & operator=(Model & rhs) = delete;
  Model & operator=(Model && rhs);

  void draw(MTL::RenderCommandEncoder * const renderCommandEncoder, MTL::RenderPipelineState* renderPipelineState) const override;

private:
  Textures* _textures;
};

#endif /* Model_hpp */
