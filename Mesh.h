#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include <QString>
#include <QOpenGLExtraFunctions>

#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"

class Renderer;

class Mesh : public Transform, private QOpenGLExtraFunctions
{
public:
    Mesh(QString filename, std::shared_ptr<Renderer> renderer);
    ~Mesh();

    int getVertexCount() const;

    void setTexture(std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> getTexture();

    void setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess);
    glm::vec4 getMaterialParams() const;

    GLuint getVertexArrayObject() const;
    GLuint getShadowVertexArrayObject() const;

private:
    void loadGeometryFromFile(QString filename);
    void prepareRendering(std::shared_ptr<Renderer> renderer);

    std::vector<glm::vec3> mVertexes;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mTexCoords;

    glm::vec4 mMaterialParams = {1, 1, 1, 128};

    std::shared_ptr<Texture> mTexture;

    GLuint mVertexArrayObject, mShadowVertexArrayObject;
    GLuint mVertexArrayBuffer, mTexCoordsArrayBuffer, mNormalArrayBuffer;
};

#endif // MESH_H
