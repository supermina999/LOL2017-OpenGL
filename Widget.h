#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <vector>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

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

    Mesh mMesh;
    Light mLight;
    Camera mCamera;

    GLuint mVertexAttrib, mTexCoordAttrib, mNormalAttrib;
    GLuint mTexture;
    GLuint mMVMatrixUniform, mMVPMatrixUniform, mNormalMatrixUniform, mLightPositionUniform, mLightingParamsUniform;
    GLuint mSamplerUniform, mShadowSamplerUniform, mShadowMatrixUniform;

    float mRotationSpeed;

    GLuint mShadowProgram, mShadowVertexShader, mShadowFragmentShader, mShadowVertexArrayObject;
    GLuint mLightMVPMatrixUniform, mShadowVertexAttrib;
    GLuint mShadowFramebuffer, mShadowTexture;
    int mShadowWidth, mShadowHeight;
    glm::mat4 mLightMVPMatrix, mShadowMatrix;
};

#endif // WIDGET_H
