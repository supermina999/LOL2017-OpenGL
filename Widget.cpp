#include "Widget.h"

#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QScreen>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    mRotationSpeed = 2.5;

    auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000 / 60);
}

void Widget::onTimer()
{
    for(auto cubeMesh : mCubeMeshes)
    {
        auto rotation = cubeMesh->getRotation();
        rotation.x += 0.03 * mRotationSpeed;
        rotation.y += 0.01 * mRotationSpeed;
        rotation.z += 0.01 * mRotationSpeed;
        cubeMesh->setRotation(rotation);
    }

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
    auto size = QApplication::primaryScreen()->availableSize() * QApplication::primaryScreen()->devicePixelRatio() * 2;
    mRenderer = std::make_shared<Renderer>(glm::vec3(0, 0, 1), size.width(), size.height());

    mTexture = std::make_shared<Texture>(":/Resources/texture.png");

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            auto cubeMesh = std::make_shared<Mesh>(":/Resources/cube.mesh", mRenderer);
            cubeMesh->setMaterialParams(0.2 + i * 0.2, 0.2 + i * 0.2, 0.1 + j * 0.2, 128 * (j + 3));
            cubeMesh->setTexture(mTexture);
            cubeMesh->setPosition({-3 + i * 3, -1.5 + j * 1.5, 0});
            cubeMesh->setRotation({0, 0, 0});
            cubeMesh->setScale({0.6, 0.4, 0.3});
            mCubeMeshes.push_back(cubeMesh);
            mMeshes.push_back(cubeMesh);
        }
    }

    mPlaneMesh = std::make_shared<Mesh>(":Resources/plane.mesh", mRenderer);
    mPlaneMesh->setMaterialParams(0.4, 0.6, 0.6, 1024);
    mPlaneMesh->setTexture(mTexture);
    mPlaneMesh->setPosition({0, 0, -2});
    mPlaneMesh->setScale({8.5, 4, 1});
    mMeshes.push_back(mPlaneMesh);

    mLight = std::make_shared<Light>();
    mLight->setPosition({1, 1, 4});

    mCamera = std::make_shared<Camera>();
    mCamera->setPosition({0, 0, 6});
}

void Widget::paintGL()
{
    mRenderer->renderShadows(mMeshes, mLight);

    mRenderer->render(mMeshes, mCamera, mLight, defaultFramebufferObject());
}

void Widget::resizeGL(int width, int height)
{
    mCamera->setAspect(static_cast<float>(width) / height);
    mRenderer->setScreenSize(width * devicePixelRatioF(), height * devicePixelRatioF());
}
