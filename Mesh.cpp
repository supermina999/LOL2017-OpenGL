#include "Mesh.h"

#include <QFile>
#include <QImage>
#include <QTextStream>
#include <QOpenGLExtraFunctions>

#include "Renderer.h"

Mesh::Mesh(QString filename, std::shared_ptr<Renderer> renderer)
{
    initializeOpenGLFunctions();
    loadGeometryFromFile(filename);
    prepareRendering(renderer);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &mVertexArrayObject);
    glDeleteVertexArrays(1, &mShadowVertexArrayObject);
    glDeleteBuffers(1, &mVertexArrayBuffer);
    glDeleteBuffers(1, &mTexCoordsArrayBuffer);
    glDeleteBuffers(1, &mNormalArrayBuffer);
}

void Mesh::loadGeometryFromFile(QString filename)
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

int Mesh::getVertexCount() const
{
    return mVertexes.size();
}

void Mesh::setTexture(std::shared_ptr<Texture> texture)
{
    mTexture = texture;
}

std::shared_ptr<Texture> Mesh::getTexture()
{
    return mTexture;
}

void Mesh::setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess)
{
    mMaterialParams = {ambientCoef, diffuseCoef, specularCoef, shininess};
}

glm::vec4 Mesh::getMaterialParams() const
{
    return mMaterialParams;
}

void Mesh::prepareRendering(std::shared_ptr<Renderer> renderer)
{
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    auto program = renderer->getProgram();
    auto shadowProgram = renderer->getShadowProgram();

    auto vertexAttrib = glGetAttribLocation(program, "vertex");
    glGenBuffers(1, &mVertexArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertexes.size() * sizeof(mVertexes[0]), mVertexes.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttrib);

    auto normalAttrib = glGetAttribLocation(program, "normal");
    glGenBuffers(1, &mNormalArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(mNormals[0]), mNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(normalAttrib);

    auto texCoordsAttrib = glGetAttribLocation(program, "texCoord");
    glGenBuffers(1, &mTexCoordsArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordsArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mTexCoords.size() * sizeof(mTexCoords[0]), mTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(texCoordsAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texCoordsAttrib);

    auto shadowVertexAttrib = glGetAttribLocation(shadowProgram, "vertex");
    glGenVertexArrays(1, &mShadowVertexArrayObject);
    glBindVertexArray(mShadowVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    glVertexAttribPointer(shadowVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shadowVertexAttrib);
}

GLuint Mesh::getVertexArrayObject() const
{
    return mVertexArrayObject;
}

GLuint Mesh::getShadowVertexArrayObject() const
{
    return mShadowVertexArrayObject;
}
