#include "camera.h"

using namespace vax::objects;
using namespace vax;

static constexpr float epsilon = 0.00001f;

void Camera::setViewPortSize(vax::math::SizeUI viewPortSize) {
    _viewPortSize = viewPortSize;
    _whAspectRatio = viewPortSize.whRatio();
    _isProjectionDirty = true;
}

void Camera::setProjection(Projection projection) {
    _projection = projection;
    _isProjectionDirty = true;
}

void Camera::setPosition(glm::vec3 position) {
    _position = position;
    _isViewDirty = true;
}

void Camera::setRotation(glm::vec3 rotation) {
    _rotation = rotation;
    _isViewDirty = true;
}

void Camera::setTarget(glm::vec3 target) {
    _target = target;
    _isViewDirty = true;
}

void Camera::setFov(vax::math::Radian fov) {
    _fov = fov;
    _isProjectionDirty = true;
}

void Camera::setNearPlane(double nearPlane) {
    _nearPlane = nearPlane;
    _isProjectionDirty = true;
}

void Camera::setFarPlane(double farPlane) {
    _farPlane = farPlane;
    _isProjectionDirty = true;
}

void Camera::setViewSize(double viewSize) {
    _viewSize = viewSize;
    if (_projection == Projection::orthographic) {
        _isProjectionDirty = true;
    }
}

glm::mat4 Camera::projectionMatrix() {
    if (_isProjectionDirty) {
        updateProjectionMatrix();
    }
    return _savedProjectionMatrix;
}

glm::mat4 Camera::viewMatrix() {
    if (_isViewDirty) {
        updateViewMatrix();
    }
    return _savedViewMatrix;
}

void Camera::updateViewMatrix() {
    float distance = glm::distance(_position, _target);
    if (glm::abs(distance) < epsilon) {
        // TODO: Check if this is correct
        glm::mat4 rotationMat = vax::math::eulerAngleXYZRotationMatrix(_rotation);
        _savedViewMatrix = glm::transpose(rotationMat) * glm::translate(glm::mat4(1.0f), -_position);
    }
    else {
        _savedViewMatrix = glm::lookAt(_position, _target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Camera::updateProjectionMatrix() {
    if (_whAspectRatio == 0) {
        return;
    }
    switch (_projection) {
    case Projection::perspective:
        _savedProjectionMatrix = glm::perspective(
            _fov,
            static_cast<float>(_whAspectRatio),
            static_cast<float>(_nearPlane),
            static_cast<float>(_farPlane)
        );
        // _savedProjectionMatrix[1][1] *= -1;
        break;
    case Projection::orthographic:
        _savedProjectionMatrix = glm::ortho(
            -_viewSize * _whAspectRatio * 0.5,
            _viewSize * _whAspectRatio * 0.5,
            -_viewSize * 0.5,
            _viewSize * 0.5,
            _nearPlane,
            _farPlane
        );
        // _savedProjectionMatrix[1][1] *= -1;
        break;
    }
}

UniformBufferObject Camera::getUniformBufferObject() {
    return {
        .view = viewMatrix(),
        .proj = projectionMatrix(),
        .cameraPosition = glm::vec4(_position, 1.0f)
    };
}

void Camera::setRotationSpeed(float rotationSpeed) {
    _rotationSpeed = rotationSpeed;
}

void Camera::rotateBy(glm::vec2 delta) {
    // transform.rotation.setY(transform.rotation.y() + delta.x * 0.05);
    // transform.rotation.setX(transform.rotation.x() + (-delta.y) * 0.05);
    _rotation.y += delta.x * _rotationSpeed;
    _rotation.x += (-delta.y) * _rotationSpeed;
    _rotation.x = std::max(
        static_cast<float>(-M_PI_2 + epsilon),
        std::min(_rotation.x, static_cast<float>(M_PI_2 - epsilon))
    );
    // transform.rotation.setX(
    //     std::max((float)-M_PI_2 + 0.01f,
    //         std::min(transform.rotation.x(), (float)M_PI_2 - 0.01f)));
    auto distanceToOrigin = glm::length(_position);
    auto rotationX = glm::rotate(glm::mat4(1.0f), _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    auto rotationY = glm::rotate(glm::mat4(1.0f), _rotation.y, glm::vec3(0.0f, -1.0f, 0.0f));
    auto rotateMatrix = rotationY * rotationX;
    auto distanceVector = glm::vec4(0.0f, 0.0f, distanceToOrigin, 0.0f);
    auto rotatedVector = rotateMatrix * distanceVector;
    _position = glm::vec3(rotatedVector.x, rotatedVector.y, rotatedVector.z);
    _isViewDirty = true;
    // auto distanceToOrigin = simd_length(transform.position);
    // auto rotationX = matrix4x4_rotation(transform.rotation.angles[0],
    //     (vector_float3) {
    //     1, 0, 0
    // });
    // auto rotationY = matrix4x4_rotation(transform.rotation.angles[1],
    //     (vector_float3) {
    //     0, 1, 0
    // });
    // auto rotateMatrix = matrix_multiply(rotationY, rotationX);
    // auto distanceVector = simd_make_float4(0.0f, 0.0f, distanceToOrigin, 0.0f);
    // auto rotatedVector = matrix_multiply(rotateMatrix, distanceVector);
    // transform.position = rotatedVector.xyz; // simd_make_float3(rotatedVector.xy, transform.position.z);
}