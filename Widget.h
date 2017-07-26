#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <vector>

#include <glm/glm.hpp>

class Widget : public QOpenGLWidget, QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void onTimer();
    void slower();
    void faster();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    GLuint createShader(QString filename, GLenum shaderType);
    void updateTransform();

    GLuint mProgram, mVertexShader, mFragmentShader;
    GLuint mVertexArrayObject;
    GLuint mVertexArrayBuffer, mTexCoordsArrayBuffer, mNormalArrayBuffer;
    std::vector<glm::vec3> mVertexes, mNormals;
    std::vector<glm::vec2> mTexCoords;
    glm::vec3 mLightPosition;
    glm::vec4 mLightingParams;

    GLuint mVertexAttrib, mTexCoordAttrib, mNormalAttrib;
    GLuint mTexture;
    GLuint mMVMatrixUniform, mMVPMatrixUniform, mNormalMatrixUniform, mLightPositionUniform, mLightingParamsUniform;
    GLuint mSamplerUniform, mShadowSamplerUniform, mShadowMatrixUniform;

    glm::vec3 mPosition, mScale, mRotation;
    glm::mat4 mMVMatrix, mMVPMatrix;
    glm::mat3 mNormalMatrix;
    int mWidth, mHeight;

    float mRotationSpeed;


    GLuint mShadowProgram, mShadowVertexShader, mShadowFragmentShader, mShadowVertexArrayObject;
    GLuint mLightMVPMatrixUniform, mShadowVertexAttrib;
    GLuint mShadowFramebuffer, mShadowTexture;
    int mShadowWidth, mShadowHeight;
    glm::mat4 mLightMVPMatrix, mShadowMatrix;
};

#endif // WIDGET_H
