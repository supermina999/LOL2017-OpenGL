#include "Widget.h"

#include <QTimer>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    mMesh.setMaterialParams(0.4, 0.6, 0.6, 1024);
    mLight.setPosition({1, 1, 4});
    mCamera.setPosition({0, 0, 6});

    mRotationSpeed = 1;

    auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000 / 60);
}

void Widget::onTimer()
{
    auto rotation = mMesh.getRotation();
    rotation.x += 0.03 * mRotationSpeed;
    rotation.y += 0.01 * mRotationSpeed;
    rotation.z += 0.01 * mRotationSpeed;
    mMesh.setRotation(rotation);

    update();
}

void Widget::slower()
{
    mRotationSpeed *= 0.8;
}

void Widget::faster()
{
    mRotationSpeed *= 1.2;
}

void Widget::initializeGL()
{
    mRenderer.initialize();

    mMesh.loadGeometryFromFile(":/Resources/cube.mesh");
    mMesh.loadTextureFromFile(":/Resources/texture.jpg");
    mMesh.prepareRendering(mRenderer);
}

void Widget::paintGL()
{
    mRenderer.renderShadows({mMesh}, mLight);

    mRenderer.render({mMesh}, mCamera, mLight, defaultFramebufferObject());
}

void Widget::resizeGL(int width, int height)
{
    mCamera.setAspect(static_cast<float>(width) / height);
    mRenderer.setScreenSize(width * devicePixelRatioF(), height * devicePixelRatioF());
}
