#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

#include <QString>

#include <QOpenGLExtraFunctions>

#include "Transform.h"
#include "Camera.h"
#include "Light.h"

class Renderer;

class Mesh : public Transform, private QOpenGLExtraFunctions
{
public:
    Mesh();

    void loadGeometryFromFile(QString fileName);
    int getVertexCount() const;

    void loadTextureFromFile(QString fileName);
    GLuint getTexture() const;

    void setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess);
    glm::vec4 getMaterialParams() const;

    void prepareRendering(const Renderer& renderer);

    GLuint getVertexArrayObject() const;
    GLuint getShadowVertexArrayObject() const;

private:
    void initializeFunctions();

    bool mIsFunctionsInitalized = false;

    std::vector<glm::vec3> mVertexes;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mTexCoords;

    glm::vec4 mMaterialParams = {1, 1, 1, 128};

    GLuint mTexture = -1;

    GLuint mVertexArrayObject, mShadowVertexArrayObject;
    GLuint mVertexArrayBuffer, mTexCoordsArrayBuffer, mNormalArrayBuffer;
};

#endif // MESH_H
