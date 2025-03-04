//
// Created by Alexander Lakhonin on 12.02.2025.
//

#include "Camera.hpp"

matrix_float4x4 Camera::viewMatrix() const {
  return matrix_look_at_left_hand(transform.position, (vector_float3){0.0f, 0.0f, 0.0f}, (vector_float3){0.0f, 1.0f, 0.0f});
}

matrix_float4x4 Camera::projectionMatrix() const {
  switch (projection) {
    case Projection::orthographic:
      return matrix_perspective_left_hand(fov, aspectRatio, nearPlane, farPlane);
      break;

    case Projection::perspective:
      return matrix_perspective_left_hand(fov, aspectRatio, nearPlane, farPlane);
      break;
  }
}
