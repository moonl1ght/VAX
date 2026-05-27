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
    _updateRotationForPosition();
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
        _updateProjectionMatrix();
    }
    return _savedProjectionMatrix;
}

glm::mat4 Camera::viewMatrix() {
    if (_isViewDirty) {
        _updateViewMatrix();
    }
    return _savedViewMatrix;
}

void Camera::_updateViewMatrix() {
    float distance = glm::distance(_position, _target);
    if (glm::abs(distance) < epsilon) {
        glm::mat4 rotationMat = vax::math::eulerAngleXYZRotationMatrix(_rotation);
        _savedViewMatrix = glm::transpose(rotationMat) * glm::translate(glm::mat4(1.0f), -_position);
    } else {
        _savedViewMatrix = glm::lookAt(_position, _target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void Camera::_updateProjectionMatrix() {
    if (_whAspectRatio == 0) {
        return;
    }
    switch (_projection) {
    case Projection::perspective:
        _savedProjectionMatrix = glm::perspective(
            _fov, static_cast<float>(_whAspectRatio), static_cast<float>(_nearPlane), static_cast<float>(_farPlane)
        );
        break;
    case Projection::orthographic:
        _savedProjectionMatrix = glm::ortho(
            -_viewSize * _whAspectRatio, _viewSize * _whAspectRatio, -_viewSize, _viewSize, _nearPlane, _farPlane
        );
        break;
    }
}

UniformBufferObject Camera::getUniformBufferObject() {
    return {
        .view = viewMatrix(),
        .proj = projectionMatrix(),
        .cameraPosition = glm::vec4(_position, 1.0f),
        .environmentMapIndex = NO_ENVIRONMENT_MAP_INDEX,
    };
}
void Camera::setRotationSpeed(float rotationSpeed) { _rotationSpeed = rotationSpeed; }

void Camera::rotateBy(glm::vec2 delta) {
    _rotation.y += delta.x * _rotationSpeed;
    _rotation.x += (-delta.y) * _rotationSpeed;
    _rotation.x =
        std::max(static_cast<float>(-M_PI_2 + epsilon), std::min(_rotation.x, static_cast<float>(M_PI_2 - epsilon)));
    auto distanceToOrigin = glm::length(_position);
    auto rotationX = glm::rotate(glm::mat4(1.0f), _rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    auto rotationY = glm::rotate(glm::mat4(1.0f), _rotation.y, glm::vec3(0.0f, -1.0f, 0.0f));
    auto rotateMatrix = rotationY * rotationX;
    auto distanceVector = glm::vec4(0.0f, 0.0f, distanceToOrigin, 0.0f);
    auto rotatedVector = rotateMatrix * distanceVector;
    _position = glm::vec3(rotatedVector.x, rotatedVector.y, rotatedVector.z);
    _isViewDirty = true;
}

void Camera::_updateRotationForPosition() {
    auto x = _position.x;
    auto y = _position.y;
    auto z = _position.z;
    _rotation.y = -std::atan2(x, z);
    float groundDist = std::sqrt(x * x + z * z);
    _rotation.x = std::atan2(-y, groundDist);
}