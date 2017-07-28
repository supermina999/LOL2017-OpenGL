#include "Transform.h"

#include <glm/gtx/transform.hpp>

Transform::Transform()
{
}

glm::vec3 Transform::getPosition() const
{
    return mPosition;
}

void Transform::setPosition(glm::vec3 position)
{
    mPosition = position;
}

glm::vec3 Transform::getScale() const
{
    return mScale;
}

void Transform::setScale(glm::vec3 scale)
{
    mScale = scale;
}

glm::vec3 Transform::getRotation() const
{
    return mRotation;
}

void Transform::setRotation(glm::vec3 rotation)
{
    mRotation = rotation;
}

glm::mat4 Transform::getModelMatrix() const
{
    auto translateMat = glm::translate(mPosition);
    auto scaleMat = glm::scale(mScale);
    auto rotateMatX = glm::rotate(mRotation.x, glm::vec3(1, 0, 0));
    auto rotateMatY = glm::rotate(mRotation.y, glm::vec3(0, 1, 0));
    auto rotateMatZ = glm::rotate(mRotation.z, glm::vec3(0, 0, 1));
    auto rotationMat = rotateMatZ * rotateMatY * rotateMatX;
    return translateMat * rotationMat * scaleMat;
}

glm::mat4 Transform::getViewMatrix() const
{
    auto translateMat = glm::translate(-mPosition);
    auto scaleMat = glm::scale(glm::vec3(1 / mScale.x, 1 / mScale.y, 1 / mScale.z));
    auto rotateMatX = glm::rotate(-mRotation.x, glm::vec3(1, 0, 0));
    auto rotateMatY = glm::rotate(-mRotation.y, glm::vec3(0, 1, 0));
    auto rotateMatZ = glm::rotate(-mRotation.z, glm::vec3(0, 0, 1));
    auto rotationMat = rotateMatZ * rotateMatY * rotateMatX;
    return translateMat * rotationMat * scaleMat;
}
