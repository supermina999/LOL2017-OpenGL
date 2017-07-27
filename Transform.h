#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

class Transform
{
public:
    Transform();

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    glm::vec3 getScale();
    void setScale(glm::vec3 scale);

    glm::vec3 getRotation();
    void setRotation(glm::vec3 rotation);

    glm::mat4 getModelMatrix();
    glm::mat4 getViewMatrix();

private:
    glm::vec3 mPosition = {0, 0, 0};
    glm::vec3 mScale = {1, 1, 1};
    glm::vec3 mRotation = {0, 0, 0};
};

#endif // TRANSFORM_H
