#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
    Light();

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    void setNear(float near);
    float getNear();

    void setFar(float far);
    float getFar();

    void setLightParams(float ambientCoef, float diffuseCoef, float specularCoef);
    glm::vec3 getLightParams();

    glm::mat4 getViewProjectionMatrix();

private:
    glm::vec3 mPosition = {0, 0, 0};
    glm::vec3 mLightParams = {1, 1, 1};
    float mNear = 1;
    float mFar = 15;
};

#endif // LIGHT_H
