//
// Created by Alexander Lakhonin on 05.02.2025.
//

#ifndef Primitives_hpp
#define Primitives_hpp

#include <stdio.h>
#include <Metal/Metal.hpp>
#include "Mesh.hpp"
#include "Model.hpp"
#include "PrimitiveModel.hpp"

namespace Primitives {

  Model createRGBTriangle(MTL::Device& device);

  Model createRGBCube(MTL::Device& device);

  std::unique_ptr<PrimitiveModel> createGizmo(MTL::Device& device);

  std::unique_ptr<PrimitiveModel> plane(MTL::Device& device);
}

#endif /* Primitives_hpp */
