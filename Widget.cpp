#include "Widget.h"

#include <QTimer>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QImage>
#include <QMessageBox>
#include <QOpenGLContext>

#include <glm/gtx/transform.hpp>

#ifdef Q_OS_ANDROID
#define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
#define GL_TEXTURE_COMPARE_MODE GL_TEXTURE_COMPARE_MODE_EXT
#define GL_COMPARE_REF_TO_TEXTURE GL_COMPARE_REF_TO_TEXTURE_EXT
#define GL_TEXTURE_COMPARE_FUNC GL_TEXTURE_COMPARE_FUNC_EXT
#endif

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    mMesh.setMaterialParams(0.4, 0.6, 0.6, 1024);

    mLight.setPosition({1, 1, 4});

    mCamera.setPosition({0, 0, 6});

    mRotationSpeed = 1;

    mShadowWidth = 1920;
    mShadowHeight = 1080;

    auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000 / 60);
}

Widget::~Widget()
{
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
    initializeOpenGLFunctions();

    glClearColor(0, 0, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    mProgram = glCreateProgram();
    mVertexShader = createShader(":/Resources/vertex.glsl", GL_VERTEX_SHADER);
    mFragmentShader = createShader(":/Resources/fragment.glsl", GL_FRAGMENT_SHADER);
    glAttachShader(mProgram, mVertexShader);
    glAttachShader(mProgram, mFragmentShader);
    glLinkProgram(mProgram);

    mShadowProgram = glCreateProgram();
    mShadowVertexShader = createShader(":/Resources/shadow_vertex.glsl", GL_VERTEX_SHADER);
    mShadowFragmentShader = createShader(":/Resources/shadow_fragment.glsl", GL_FRAGMENT_SHADER);
    glAttachShader(mShadowProgram, mShadowVertexShader);
    glAttachShader(mShadowProgram, mShadowFragmentShader);
    glLinkProgram(mShadowProgram);

    glGenFramebuffers(1, &mShadowFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFramebuffer);

    glGenTextures(1, &mShadowTexture);
    glBindTexture(GL_TEXTURE_2D, mShadowTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, mShadowWidth, mShadowHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowTexture, 0);    


    mMesh.loadGeometryFromFile(":/Resources/cube.mesh");
    mMesh.loadTextureFromFile(":/Resources/texture.jpg");
    mMesh.prepareRendering(mProgram, mShadowProgram);
}

void Widget::paintGL()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFramebuffer);
    glViewport(0, 0, mShadowWidth, mShadowHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2, 1);
    glCullFace(GL_FRONT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glUseProgram(mShadowProgram);

    mMesh.renderShadow(mShadowProgram, mLight);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);




    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, width() * devicePixelRatioF(), height() * devicePixelRatioF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glUseProgram(mProgram);

    auto shadowSamplerUniform = glGetUniformLocation(mProgram, "shadowSampler");
    glUniform1i(shadowSamplerUniform, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mShadowTexture);

    auto lightPosition = mLight.getPosition();
    auto lightPositionUniform = glGetUniformLocation(mProgram, "lightPosition");
    glUniform3fv(lightPositionUniform, 1, &lightPosition[0]);

    mMesh.render(mProgram, mCamera, mLight);
}

void Widget::resizeGL(int w, int h)
{
    mCamera.setAspect(static_cast<float>(w) / h);
}

GLuint Widget::createShader(QString filename, GLenum shaderType)
{
    auto shader = glCreateShader(shaderType);
    QFile sourceFile(filename);
    sourceFile.open(QIODevice::ReadOnly);
    auto source = QTextStream(&sourceFile).readAll();

    if(context()->isOpenGLES())
    {
        source = "#version 300 es\n"
                 + source;
    }
    else
    {
        source = "#version 330 core\n"
                 + source;
    }

    int sourceLength = source.length();
    auto sourceStdString = source.toStdString();
    auto sourceCStr = sourceStdString.c_str();
    glShaderSource(shader, 1, &sourceCStr, &sourceLength);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        QMessageBox::warning(this, "Can't compile shader", source);
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            auto infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, 0, infoLog);
            QString str(infoLog);
            QMessageBox::warning(this, "Can't compile shader", str);

            delete[] infoLog;
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
