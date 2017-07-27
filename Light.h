#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
    Light();

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    void setLightParams(float ambientCoef, float diffuseCoef, float specularCoef);
    glm::vec3 getLightParams();

private:
    glm::vec3 mPosition = {0, 0, 0};
    glm::vec3 mLightParams = {1, 1, 1};
};

#endif // LIGHT_H
