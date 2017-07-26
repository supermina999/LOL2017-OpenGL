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
    mPosition = {0, 0, -6};
    mScale = {1, 1, 1};
    mRotation = {0, 0, 0};
    mRotationSpeed = 1;

    mShadowWidth = 1920;
    mShadowHeight = 1080;

    mVertexes = {
        {1, 1, 1}, {-1, 1, 1}, {-1, -1, 1},
        {1, 1, 1}, {-1, -1, 1}, {1, -1, 1},
        {1, 1, -1}, {1, -1, -1}, {-1, -1, -1},
        {1, 1, -1}, {-1, -1, -1}, {-1, 1, -1},
        {1, 1, 1}, {1, 1, -1}, {-1, 1, -1},
        {1, 1, 1}, {-1, 1, -1}, {-1, 1, 1},
        {1, -1, 1}, {-1, -1, 1}, {-1, -1, -1},
        {1, -1, 1}, {-1, -1, -1}, {1, -1, -1},
        {1, 1, 1}, {1, -1, 1}, {1, -1, -1},
        {1, 1, 1}, {1, -1, -1}, {1, 1, -1},
        {-1, 1, 1}, {-1, 1, -1}, {-1, -1, -1},
        {-1, 1, 1}, {-1, -1, -1}, {-1, -1, 1},
        {2, 2, -2}, {-2, 2, -2}, {-2, -2, -2},
        {-2, -2, -2}, {2, -2, -2}, {2, 2, -2},
    };

    mNormals = {
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
        {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
        {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
        {0, 1, 0}, {0, 1, 0}, {0, 1, 0},
        {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
        {0, -1, 0}, {0, -1, 0}, {0, -1, 0},
        {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
        {1, 0, 0}, {1, 0, 0}, {1, 0, 0},
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
        {0, 0, 1}, {0, 0, 1}, {0, 0, 1},
    };

    mTexCoords = {
        {1, 1}, {0, 1}, {0, 0},
        {1, 1}, {0, 0}, {1, 0},
        {1, 1}, {1, 0}, {0, 0},
        {1, 1}, {0, 0}, {0, 1},
        {1, 1}, {0, 1}, {0, 0},
        {1, 1}, {0, 0}, {1, 0},
        {1, 1}, {1, 0}, {0, 0},
        {1, 1}, {0, 0}, {0, 1},
        {1, 1}, {0, 1}, {0, 0},
        {1, 1}, {0, 0}, {1, 0},
        {1, 1}, {1, 0}, {0, 0},
        {1, 1}, {0, 0}, {0, 1},
        {0, 0}, {0, 0}, {0, 0},
        {0, 0}, {0, 0}, {0, 0},
    };

    mLightPosition = {1, 1, 4};
    mLightingParams = {0.4, 0.6, 0.6, 1024};

    auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000 / 60);
}

Widget::~Widget()
{
    makeCurrent();

    glDetachShader(mProgram, mVertexShader);
    glDetachShader(mProgram, mFragmentShader);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragmentShader);
    glDeleteProgram(mProgram);

    glDeleteBuffers(1, &mVertexArrayBuffer);
    glDeleteBuffers(1, &mNormalArrayBuffer);
    glDeleteBuffers(1, &mTexCoordsArrayBuffer);
    glDeleteTextures(1, &mTexture);
    glDeleteVertexArrays(1, &mVertexArrayObject);

    doneCurrent();
}

void Widget::onTimer()
{
    mRotation.x += 0.03 * mRotationSpeed;
    mRotation.y += 0.01 * mRotationSpeed;
    mRotation.z += 0.01 * mRotationSpeed;

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

    mMVMatrixUniform = glGetUniformLocation(mProgram, "mvMatrix");
    mMVPMatrixUniform = glGetUniformLocation(mProgram, "mvpMatrix");
    mNormalMatrixUniform = glGetUniformLocation(mProgram, "normalMatrix");
    mLightPositionUniform = glGetUniformLocation(mProgram, "lightPosition");
    mShadowMatrixUniform = glGetUniformLocation(mProgram, "shadowMatrix");
    mSamplerUniform = glGetUniformLocation(mProgram, "sampler");
    mShadowSamplerUniform = glGetUniformLocation(mProgram, "shadowSampler");

    mLightingParamsUniform = glGetUniformLocation(mProgram, "lightingParams");

    mVertexAttrib = glGetAttribLocation(mProgram, "vertex");
    mTexCoordAttrib = glGetAttribLocation(mProgram, "texCoord");
    mNormalAttrib = glGetAttribLocation(mProgram, "normal");

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    glGenBuffers(1, &mVertexArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertexes.size() * sizeof(mVertexes[0]), mVertexes.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mVertexAttrib);

    glGenBuffers(1, &mNormalArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(mNormals[0]), mNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mNormalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mNormalAttrib);

    glGenBuffers(1, &mTexCoordsArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordsArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mTexCoords.size() * sizeof(mTexCoords[0]), mTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mTexCoordAttrib);

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto image = QImage(":/Resources/texture.jpg").convertToFormat(QImage::Format_RGBA8888);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());



    mShadowProgram = glCreateProgram();
    mShadowVertexShader = createShader(":/Resources/shadow_vertex.glsl", GL_VERTEX_SHADER);
    mShadowFragmentShader = createShader(":/Resources/shadow_fragment.glsl", GL_FRAGMENT_SHADER);
    glAttachShader(mShadowProgram, mShadowVertexShader);
    glAttachShader(mShadowProgram, mShadowFragmentShader);
    glLinkProgram(mShadowProgram);

    mLightMVPMatrixUniform = glGetUniformLocation(mShadowProgram, "mvpMatrix");
    mShadowVertexAttrib = glGetAttribLocation(mShadowProgram, "vertex");

    glGenVertexArrays(1, &mShadowVertexArrayObject);
    glBindVertexArray(mShadowVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    glVertexAttribPointer(mShadowVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(mShadowVertexAttrib);

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
}

void Widget::resizeGL(int w, int h)
{
    mWidth = w;
    mHeight = h;
}

void Widget::paintGL()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFramebuffer);
    glViewport(0, 0, mShadowWidth, mShadowHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2, 1);
    glCullFace(GL_FRONT);

    glUseProgram(mShadowProgram);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glBindVertexArray(mShadowVertexArrayObject);

    updateTransform();
    glUniformMatrix4fv(mLightMVPMatrixUniform, 1, GL_FALSE, &mLightMVPMatrix[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, mVertexes.size() - 6);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);




    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, mWidth * devicePixelRatioF(), mHeight * devicePixelRatioF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glUseProgram(mProgram);

    glBindVertexArray(mVertexArrayObject);

    glUniform1i(mSamplerUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glUniform1i(mShadowSamplerUniform, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mShadowTexture);

    updateTransform();
    glUniformMatrix4fv(mMVMatrixUniform, 1, GL_FALSE, &mMVMatrix[0][0]);
    glUniformMatrix4fv(mMVPMatrixUniform, 1, GL_FALSE, &mMVPMatrix[0][0]);
    glUniformMatrix3fv(mNormalMatrixUniform, 1, GL_FALSE, &mNormalMatrix[0][0]);
    glUniformMatrix4fv(mShadowMatrixUniform, 1, GL_FALSE, &mShadowMatrix[0][0]);

    glUniform3fv(mLightPositionUniform, 1, &mLightPosition[0]);
    glUniform4fv(mLightingParamsUniform, 1, &mLightingParams[0]);

    glDrawArrays(GL_TRIANGLES, 0, mVertexes.size());
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

void Widget::updateTransform()
{
    auto translateMat = glm::translate(mPosition);
    auto scaleMat = glm::scale(mScale);
    auto rotateMatX = glm::rotate(mRotation.x, glm::vec3(1, 0, 0));
    auto rotateMatY = glm::rotate(mRotation.y, glm::vec3(0, 1, 0));
    auto rotateMatZ = glm::rotate(mRotation.z, glm::vec3(0, 0, 1));
    auto rotationMat = rotateMatZ * rotateMatY * rotateMatX;

    float perspectiveFov = 70;
    float perspectiveNear = 0.01;
    float perspectiveFar = 15;
    float aspect = static_cast<float>(mWidth) / mHeight;
    auto perspectiveMat = glm::perspective(perspectiveFov, aspect, perspectiveNear, perspectiveFar);

    mMVMatrix = translateMat * rotationMat * scaleMat;
    mMVPMatrix = perspectiveMat * mMVMatrix;
    mNormalMatrix = glm::inverse(glm::transpose(glm::mat3(mMVMatrix)));

    auto lightViewMatrix = glm::lookAt(mLightPosition, {0, 0, 0}, {0, 1, 0});
    auto lightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 20.0f);

    glm::mat4 scaleBiasMatrix = {
        {0.5, 0, 0, 0},
        {0, 0.5, 0, 0},
        {0, 0, 0.5, 0},
        {0.5, 0.5, 0.5, 1}
    };
    mLightMVPMatrix = lightProjectionMatrix * lightViewMatrix * rotationMat * scaleMat;
    mShadowMatrix = scaleBiasMatrix * mLightMVPMatrix;
}
