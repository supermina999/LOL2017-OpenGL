#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>

#include <vector>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Renderer.h"
#include "Texture.h"

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

    std::shared_ptr<Renderer> mRenderer;

    std::vector<std::shared_ptr<Mesh>> mCubeMeshes;
    std::vector<std::shared_ptr<Mesh>> mMeshes;
    std::shared_ptr<Mesh> mPlaneMesh;
    std::shared_ptr<Texture> mTexture;
    std::shared_ptr<Light> mLight;
    std::shared_ptr<Camera> mCamera;

    float mRotationSpeed;
};

#endif // WIDGET_H
