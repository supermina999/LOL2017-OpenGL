#include "Mesh.h"

#include <QFile>
#include <QImage>
#include <QTextStream>
#include <QOpenGLExtraFunctions>

#include <iostream>
using namespace std;

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

void Mesh::setMaterialParams(float ambientCoef, float diffuseCoef, float specularCoef, float shininess)
{
    mMaterialParams = {ambientCoef, diffuseCoef, specularCoef, shininess};
}

glm::vec4 Mesh::getMaterialParams()
{
    return mMaterialParams;
}

void Mesh::prepareRendering(GLuint program, GLuint shadowProgram)
{
    QOpenGLExtraFunctions fn(QOpenGLContext::currentContext());
    fn.initializeOpenGLFunctions();

    fn.glGenVertexArrays(1, &mVertexArrayObject);
    fn.glBindVertexArray(mVertexArrayObject);

    auto vertexAttrib = fn.glGetAttribLocation(program, "vertex");
    fn.glGenBuffers(1, &mVertexArrayBuffer);
    fn.glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    fn.glBufferData(GL_ARRAY_BUFFER, mVertexes.size() * sizeof(mVertexes[0]), mVertexes.data(), GL_STATIC_DRAW);
    fn.glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    fn.glEnableVertexAttribArray(vertexAttrib);

    auto normalAttrib = fn.glGetAttribLocation(program, "normal");
    fn.glGenBuffers(1, &mNormalArrayBuffer);
    fn.glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayBuffer);
    fn.glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(mNormals[0]), mNormals.data(), GL_STATIC_DRAW);
    fn.glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    fn.glEnableVertexAttribArray(normalAttrib);

    auto texCoordsAttrib = fn.glGetAttribLocation(program, "texCoord");
    fn.glGenBuffers(1, &mTexCoordsArrayBuffer);
    fn.glBindBuffer(GL_ARRAY_BUFFER, mTexCoordsArrayBuffer);
    fn.glBufferData(GL_ARRAY_BUFFER, mTexCoords.size() * sizeof(mTexCoords[0]), mTexCoords.data(), GL_STATIC_DRAW);
    fn.glVertexAttribPointer(texCoordsAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    fn.glEnableVertexAttribArray(texCoordsAttrib);

    auto shadowVertexAttrib = fn.glGetAttribLocation(shadowProgram, "vertex");
    fn.glGenVertexArrays(1, &mShadowVertexArrayObject);
    fn.glBindVertexArray(mShadowVertexArrayObject);
    fn.glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    fn.glVertexAttribPointer(shadowVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    fn.glEnableVertexAttribArray(shadowVertexAttrib);
}

void Mesh::loadTextureFromFile(QString fileName)
{
    QOpenGLExtraFunctions fn(QOpenGLContext::currentContext());
    fn.initializeOpenGLFunctions();

    fn.glGenTextures(1, &mTexture);
    fn.glBindTexture(GL_TEXTURE_2D, mTexture);
    fn.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    fn.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    fn.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    fn.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto image = QImage(fileName).convertToFormat(QImage::Format_RGBA8888);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
}

GLuint Mesh::getTexture(GLuint texture)
{
    return texture;
}

void Mesh::renderShadow(GLuint shadowProgram, Light light)
{
    QOpenGLExtraFunctions fn(QOpenGLContext::currentContext());
    fn.initializeOpenGLFunctions();

    auto lightMVPMatrix = light.getViewProjectionMatrix() * getModelMatrix();
    auto lightMVPMatrixUniform = glGetUniformLocation(shadowProgram, "mvpMatrix");
    fn.glUniformMatrix4fv(lightMVPMatrixUniform, 1, GL_FALSE, &lightMVPMatrix[0][0]);

    cout << "LIGHT" << endl;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            cout << light.getViewProjectionMatrix()[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl << endl;

    fn.glBindVertexArray(mShadowVertexArrayObject);
    fn.glDrawArrays(GL_TRIANGLES, 0, mVertexes.size());
}

void Mesh::render(GLuint program, Camera camera, Light light)
{
    QOpenGLExtraFunctions fn(QOpenGLContext::currentContext());
    fn.initializeOpenGLFunctions();

    auto samplerUniform = fn.glGetUniformLocation(program, "sampler");
    fn.glUniform1i(samplerUniform, 0);
    fn.glActiveTexture(GL_TEXTURE0);
    fn.glBindTexture(GL_TEXTURE_2D, mTexture);

    auto mvMatrix = camera.getViewMatrix() * getModelMatrix();
    auto mvMatrixUniform = fn.glGetUniformLocation(program, "mvMatrix");
    fn.glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, &mvMatrix[0][0]);

    auto mvpMatrix = camera.getProjectionMatrix() * mvMatrix;
    auto mvpMatrixUniform = fn.glGetUniformLocation(program, "mvpMatrix");
    fn.glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, &mvpMatrix[0][0]);

    auto normalMatrix = glm::inverse(glm::transpose(glm::mat3(mvMatrix)));
    auto normalMatrixUniform = fn.glGetUniformLocation(program, "normalMatrix");
    fn.glUniformMatrix3fv(normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);

    glm::mat4 scaleBiasMatrix = {
        {0.5, 0, 0, 0},
        {0, 0.5, 0, 0},
        {0, 0, 0.5, 0},
        {0.5, 0.5, 0.5, 1}
    };
    auto lightMVPMatrix = light.getViewProjectionMatrix() * getModelMatrix();
    auto shadowMatrix = scaleBiasMatrix * lightMVPMatrix;
    auto shadowMatrixUniform = glGetUniformLocation(program, "shadowMatrix");
    fn.glUniformMatrix4fv(shadowMatrixUniform, 1, GL_FALSE, &shadowMatrix[0][0]);

    auto lightingParamsUniform = glGetUniformLocation(program, "lightingParams");
    auto lightCoefs = light.getLightParams();
    glm::vec4 lightingParams = {mMaterialParams.x * lightCoefs.x, mMaterialParams.y * lightCoefs.y,
                                mMaterialParams.z * lightCoefs.z, mMaterialParams.w};
    glUniform4fv(lightingParamsUniform, 1, &lightingParams[0]);

    fn.glBindVertexArray(mVertexArrayObject);
    fn.glDrawArrays(GL_TRIANGLES, 0, mVertexes.size());
}
