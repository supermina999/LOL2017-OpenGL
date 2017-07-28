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
    void paintGL();
    void resizeGL(int w, int h);

    GLuint createShader(QString filename, GLenum shaderType);

    GLuint mProgram, mVertexShader, mFragmentShader;

    Mesh mMesh;
    Light mLight;
    Camera mCamera;

    float mRotationSpeed;

    GLuint mShadowProgram, mShadowVertexShader, mShadowFragmentShader;
    GLuint mShadowFramebuffer, mShadowTexture;
    int mShadowWidth, mShadowHeight;
};

#endif // WIDGET_H
