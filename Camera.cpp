#include "Camera.h"

#include <glm/gtx/transform.hpp>

Camera::Camera()
{
}

void Camera::setFov(float fov)
{
    mFov = fov;
}

void Camera::setNear(float near)
{
    mNear = near;
}

void Camera::setFar(float far)
{
    mFar = far;
}

void Camera::setAspect(float aspect)
{
    mAspect = aspect;
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(mFov, mAspect, mNear, mFar);
}
