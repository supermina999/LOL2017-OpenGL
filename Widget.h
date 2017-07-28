#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>

#include <vector>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Renderer.h"

class Widget : public QOpenGLWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);

public slots:
    void onTimer();
    void slower();
    void faster();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    Renderer mRenderer;

    Mesh mMesh;
    Light mLight;
    Camera mCamera;

    float mRotationSpeed;
};

#endif // WIDGET_H
