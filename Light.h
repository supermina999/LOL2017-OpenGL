#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
    Light();

    glm::vec3 getPosition() const;
    void setPosition(glm::vec3 position);

    void setNear(float near);
    float getNear() const;

    void setFar(float far);
    float getFar() const;

    void setLightParams(float ambientCoef, float diffuseCoef, float specularCoef);
    glm::vec3 getLightParams() const;

    glm::mat4 getViewProjectionMatrix() const;

private:
    glm::vec3 mPosition = {0, 0, 0};
    glm::vec3 mLightParams = {1, 1, 1};
    float mNear = 0.7;
    float mFar = 8;
};

#endif // LIGHT_H
