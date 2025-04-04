//
// Created by Alexander Lakhonin on 02.04.2025.
//

#include "ShadowCamera.hpp"

void ShadowCamera::update(Camera camera, simd_float3 lightPosition) noexcept {
  auto nearPoints = ShadowCamera::calculatePlane(camera, camera.nearPlane);
  auto farPoints = ShadowCamera::calculatePlane(camera, camera.farPlane);

  auto radius1 = simd_distance(nearPoints.lowerLeft, farPoints.upperRight) * 0.5;
  auto radius2 = simd_distance(farPoints.lowerLeft, farPoints.upperRight) * 0.5;
  simd_float3 center;
  if (radius1 > radius2) {
    center = simd_mix(nearPoints.lowerLeft, farPoints.upperRight, (simd_float3){0.5, 0.5, 0.5});
  } else {
    center = simd_mix(farPoints.lowerLeft, farPoints.upperRight, (simd_float3){0.5, 0.5, 0.5});
  }
  auto radius = simd_max(radius1, radius2);

  auto direction = simd_normalize(lightPosition);
  transform.position = center + direction * radius;
  farPlane = radius * 2;
  nearPlane = 0.01;
  viewSize = farPlane;
  target = center;
}
