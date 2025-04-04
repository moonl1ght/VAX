//
// Created by Alexander Lakhonin on 02.04.2025.
//

#ifndef ShadowCamera_hpp
#define ShadowCamera_hpp

#include <stdio.h>
#include "Camera.hpp"

class ShadowCamera: public Camera {
public:
  ShadowCamera(): Camera() {
    projection = Projection::orthographic;
  }

  ShadowCamera(Camera camera, simd_float3 lightPosition): Camera() {
    projection = Projection::orthographic;
    update(camera, lightPosition);
  };

  void update(Camera camera, simd_float3 lightPosition) noexcept;

  static FrustumPoints calculatePlane(Camera camera, float distance) {
    float halfHeight;
    float halfWidth;
    switch (camera.projection) {
      case Camera::Projection::orthographic: {
        halfHeight = camera.viewSize * 0.5;
        halfWidth = halfHeight * camera.aspectRatio;
        break;
      }
      case Camera::Projection::perspective: {
        float halfFov = camera.fov * 0.5;
        halfHeight = tan(halfFov) * distance;
        halfWidth = halfHeight * camera.aspectRatio;
        break;
      }
    }

    return FrustumPoints::calculatePlanePoints(
      camera.viewMatrix(),
      halfHeight,
      halfWidth,
      distance,
      camera.transform.position
    );
  }
};

#endif /* ShadowCamera_hpp */
