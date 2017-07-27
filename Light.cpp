#include "Light.h"

Light::Light()
{
}

glm::vec3 Light::getPosition()
{
    return mPosition;
}

void Light::setPosition(glm::vec3 position)
{
    mPosition = position;
}

void Light::setLightParams(float ambientCoef, float diffuseCoef, float specularCoef)
{
    mLightParams = {ambientCoef, diffuseCoef, specularCoef};
}

glm::vec3 Light::getLightParams()
{
    return mLightParams;
}
