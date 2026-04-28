#pragma once

#include "luna.h"
#include "vaxMath.h"

namespace vax::objects {
    class Camera {
    public:
        enum class Projection {
            perspective, orthographic
        };

        explicit Camera(
            vax::math::SizeUI viewPortSize,
            Projection projection = Projection::perspective,
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
            vax::math::Radian fov = vax::math::radians_from_degrees(60),
            double nearPlane = 0.1,
            double farPlane = 100,
            double viewSize = 10
        )
            : _viewPortSize(viewPortSize)
            , _projection(projection)
            , _position(position)
            , _rotation(rotation)
            , _target(target)
            , _fov(fov)
            , _whAspectRatio(viewPortSize.whRatio())
            , _nearPlane(nearPlane)
            , _farPlane(farPlane)
            , _viewSize(viewSize)
        {
            updateViewMatrix();
            updateProjectionMatrix();
        };

        glm::mat4 projectionMatrix();
        glm::mat4 viewMatrix();


        void setViewPortSize(vax::math::SizeUI viewPortSize);

        void setProjection(Projection projection);

        void setPosition(glm::vec3 position);

        void setRotation(glm::vec3 rotation);

        void setTarget(glm::vec3 target);

        void setFov(vax::math::Radian fov);

        void setNearPlane(double nearPlane);

        void setFarPlane(double farPlane);

        void setViewSize(double viewSize);

        // void rotate(simd_float2 delta);
        // void setPosition(simd_float3 position);
        // void precalculateMatrices();

    private:
        Projection _projection;
        glm::vec3 _position;
        glm::vec3 _rotation;
        vax::math::Radian _fov;
        vax::math::SizeUI _viewPortSize;
        double _nearPlane;
        double _farPlane;
        double _viewSize;
        glm::vec3 _target;

        double _whAspectRatio;
        glm::mat4 _savedProjectionMatrix;
        glm::mat4 _savedViewMatrix;
        bool _isViewDirty = false;
        bool _isProjectionDirty = false;

        void updateViewMatrix();
        void updateProjectionMatrix();
    };
}