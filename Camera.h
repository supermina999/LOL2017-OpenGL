#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include "Transform.h"

class Camera : public Transform
{
public:
    Camera();

    void setFov(float fov);
    void setNear(float near);
    void setFar(float far);
    void setAspect(float aspect);

    glm::mat4 getProjectionMatrix();

private:
    float mFov = 70;
    float mNear = 0.1;
    float mFar = 100;
    float mAspect = 1;
};

#endif // CAMERA_H
