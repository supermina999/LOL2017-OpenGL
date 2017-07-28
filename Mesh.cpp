#include "Mesh.h"

#include <QFile>
#include <QImage>
#include <QTextStream>
#include <QOpenGLExtraFunctions>

#include "Renderer.h"

Mesh::Mesh()
{
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

void Mesh::setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess)
{
    mMaterialParams = {ambientCoef, diffuseCoef, specularCoef, shininess};
}

glm::vec4 Mesh::getMaterialParams() const
{
    return mMaterialParams;
}

void Mesh::prepareRendering(const Renderer &renderer)
{
    initializeFunctions();

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    auto program = renderer.getProgram();
    auto shadowProgram = renderer.getShadowProgram();

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

void Mesh::loadTextureFromFile(QString fileName)
{
    initializeFunctions();

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto image = QImage(fileName).convertToFormat(QImage::Format_RGBA8888);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
}

GLuint Mesh::getTexture() const
{
    return mTexture;
}

GLuint Mesh::getVertexArrayObject() const
{
    return mVertexArrayObject;
}

GLuint Mesh::getShadowVertexArrayObject() const
{
    return mShadowVertexArrayObject;
}

void Mesh::initializeFunctions()
{
    if(!mIsFunctionsInitalized)
    {
        initializeOpenGLFunctions();
        mIsFunctionsInitalized = true;
    }
}
