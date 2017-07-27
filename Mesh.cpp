#include "Mesh.h"

#include <QFile>
#include <QTextStream>

Mesh::Mesh()
{
}

Mesh::loadGeometryFromFile(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    int vertexCount;
    stream >> vertexCount;

    mVertexes.resize(vertexCount);
    for(int i = 0; i < vertexCount; i++)
    {
        for(int j = 0; j < 3; j++)
        {
           stream >> mVertexes[i][j];
        }
    }

    mNormals.resize(vertexCount);
    for(int i = 0; i < vertexCount; i++)
    {
        for(int j = 0; j < 3; j++)
        {
           stream >> mNormals[i][j];
        }
    }

    mTexCoords.resize(vertexCount);
    for(int i = 0; i < vertexCount; i++)
    {
        for(int j = 0; j < 2; j++)
        {
           stream >> mTexCoords[i][j];
        }
    }
}

void Mesh::setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess)
{
    mMaterialParams = {ambientCoef, diffuseCoef, specularCoef, shininess};
}
