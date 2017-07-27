#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vector>

#include <QString>

#include "Transform.h"

class Mesh : public Transform
{
public:
    Mesh();

    void loadGeometryFromFile(QString fileName);

    void setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess);
    glm::vec4 getMaterialParams();

private:
    std::vector<glm::vec3> mVertexes;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec2> mTexCoords;

    glm::vec4 mMaterialParams = {1, 1, 1, 128};
};

#endif // MESH_H
