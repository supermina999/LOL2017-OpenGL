#include "Light.h"

#include <glm/gtx/transform.hpp>

Light::Light()
{
}

glm::vec3 Light::getPosition() const
{
    return mPosition;
}

void Light::setPosition(glm::vec3 position)
{
    mPosition = position;
}

void Light::setFar(float far)
{
    mFar = far;
}

float Light::getFar() const
{
    return mFar;
}

void Light::setNear(float near)
{
    mNear = near;
}

float Light::getNear() const
{
    return mNear;
}

void Light::setLightParams(float ambientCoef, float diffuseCoef, float specularCoef)
{
    mLightParams = {ambientCoef, diffuseCoef, specularCoef};
}

glm::vec3 Light::getLightParams() const
{
    return mLightParams;
}

glm::mat4 Light::getViewProjectionMatrix() const
{
    auto lightViewMatrix = glm::lookAt(mPosition, {0, 0, 0}, {0, 1, 0});
    auto lightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, mNear, mFar);
    return lightProjectionMatrix * lightViewMatrix;
}
