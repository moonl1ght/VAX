//
// Created by Alexander Lakhonin on 12.02.2025.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "MathUtils.h"
#include "Transform.hpp"

class Camera {
  enum class Projection {
    perspective, orthographic
  };

public:

  Projection projection;
  Transform transform;
  Radian fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;

  Camera(Projection projection = Projection::perspective,
         Transform transform = Transform(), Radian fov = radians_from_degrees(60),
         float nearPlane = 0.1, float farPlane = 100)
  : projection(projection)
  , transform(transform)
  , fov(fov)
  , aspectRatio(1)
  , nearPlane(nearPlane)
  , farPlane(farPlane)
  { };

  matrix_float4x4 projectionMatrix() const;
  matrix_float4x4 viewMatrix() const;

  void rotate(simd_float2 delta);
  void setPosition(simd_float3 position);

private:
  void updateRotationForPosition();
};

#endif /* Camera_hpp */
