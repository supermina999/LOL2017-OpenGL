#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

#include <QString>

#include <QOpenGLContext>

#include "Transform.h"
#include "Camera.h"
#include "Light.h"

class Mesh : public Transform
{
public:
    Mesh();

    void loadGeometryFromFile(QString fileName);

    void loadTextureFromFile(QString fileName);
    GLuint getTexture(GLuint texture);

    void setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess);
    glm::vec4 getMaterialParams();

    void prepareRendering(GLuint program, GLuint shadowProgram);

    void renderShadow(GLuint shadowProgram, Light light);
    void render(GLuint program, Camera camera, Light light);

private:
    std::vector<glm::vec3> mVertexes;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mTexCoords;

    glm::vec4 mMaterialParams = {1, 1, 1, 128};

    GLuint mTexture = -1;

    GLuint mVertexArrayObject, mShadowVertexArrayObject;
    GLuint mVertexArrayBuffer, mTexCoordsArrayBuffer, mNormalArrayBuffer;
};

#endif // MESH_H
