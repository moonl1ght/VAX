//
// Created by Alexander Lakhonin on 26.03.2025.
//

#ifndef PrimitiveModel_hpp
#define PrimitiveModel_hpp

#include <stdio.h>

#include "BaseModel.hpp"

class PrimitiveModel: public BaseModel {
public:
  using BaseModel::BaseModel;

  void draw(MTL::RenderCommandEncoder * const renderCommandEncoder, MTL::RenderPipelineState* renderPipelineState) const override;
};

#endif /* PrimitiveModel_hpp */
