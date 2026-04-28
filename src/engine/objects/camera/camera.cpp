#include "camera.h"

using namespace vax::objects;
using namespace vax;

#define epsilon 0.00001

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
        glm::mat4 rotationMat = vax::math::eulerAngleXYZRotationMatrix(_rotation);
        _savedViewMatrix = glm::transpose(rotationMat) * glm::translate(glm::mat4(1.0f), -_position);
    }
    else {
        _savedViewMatrix = glm::lookAt(_position, _target, glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void Camera::updateProjectionMatrix() {
    switch (_projection) {
    case Projection::perspective:
        _savedProjectionMatrix = glm::perspective(
            _fov,
            static_cast<float>(_whAspectRatio),
            static_cast<float>(_nearPlane),
            static_cast<float>(_farPlane)
        );
        _savedProjectionMatrix[1][1] *= -1;
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
        _savedProjectionMatrix[1][1] *= -1;
        break;
    }
}