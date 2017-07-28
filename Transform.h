#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

class Transform
{
public:
    Transform();

    glm::vec3 getPosition() const;
    void setPosition(glm::vec3 position);

    glm::vec3 getScale() const;
    void setScale(glm::vec3 scale);

    glm::vec3 getRotation() const;
    void setRotation(glm::vec3 rotation);

    glm::mat4 getModelMatrix() const;
    glm::mat4 getViewMatrix() const;

private:
    glm::vec3 mPosition = {0, 0, 0};
    glm::vec3 mScale = {1, 1, 1};
    glm::vec3 mRotation = {0, 0, 0};
};

#endif // TRANSFORM_H
