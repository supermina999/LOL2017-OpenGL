#include "Widget.h"

#include <QTimer>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QImage>
#include <QThread>
#include <QMessageBox>
#include <QOpenGLContext>

#include <glm/gtx/transform.hpp>

Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    mAspect = 1;
    mPosition = {0, 0, -5};
    mScale = {1, 1, 1};
    mRotation = {0, 0, 0};
    mRotationSpeed = 1;
    updateTransform();

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
    };

    mLightPosition = {0, 0, 5};
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

    doneCurrent();
}

void Widget::onTimer()
{
    mRotation.x += 0.015 * mRotationSpeed;
    mRotation.y += 0.005 * mRotationSpeed;
    mRotation.z += 0.005 * mRotationSpeed;
    //scale.x += 0.001;
    updateTransform();
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

    glViewport(-1, -1, 2, 2);

    glClearColor(0, 0, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    mProgram = glCreateProgram();
    mVertexShader = createShader(":/Resources/vertex.glsl", GL_VERTEX_SHADER);
    mFragmentShader = createShader(":/Resources/fragment.glsl", GL_FRAGMENT_SHADER);
    glAttachShader(mProgram, mVertexShader);
    glAttachShader(mProgram, mFragmentShader);
    glLinkProgram(mProgram);
    glFinish();

    mMVMatrixUniform = glGetUniformLocation(mProgram, "mvMatrix");
    mMVPMatrixUniform = glGetUniformLocation(mProgram, "mvpMatrix");
    mNormalMatrixUniform = glGetUniformLocation(mProgram, "normalMatrix");
    mLightPositionUniform = glGetUniformLocation(mProgram, "lightPosition");

    mLightingParamsUniform = glGetUniformLocation(mProgram, "lightingParams");

    mVertexAttrib = glGetAttribLocation(mProgram, "vertex");
    mTexCoordAttrib = glGetAttribLocation(mProgram, "texCoord");
    mNormalAttrib = glGetAttribLocation(mProgram, "normal");

    //glGenVertexArrays(1, &vertexArrayObject);
    //glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &mVertexArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertexes.size() * sizeof(mVertexes[0]), mVertexes.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mNormalArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(mNormals[0]), mNormals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mTexCoordsArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordsArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, mTexCoords.size() * sizeof(mTexCoords[0]), mTexCoords.data(), GL_STATIC_DRAW);

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto image = QImage(":/Resources/texture.jpg").convertToFormat(QImage::Format_RGBA8888);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glFinish();
}

void Widget::resizeGL(int w, int h)
{
    mAspect = (float(w)) / h;
    updateTransform();
}

void Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexArrayBuffer);
    glEnableVertexAttribArray(mVertexAttrib);
    glVertexAttribPointer(mVertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mNormalArrayBuffer);
    glEnableVertexAttribArray(mNormalAttrib);
    glVertexAttribPointer(mNormalAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mTexCoordsArrayBuffer);
    glEnableVertexAttribArray(mTexCoordAttrib);
    glVertexAttribPointer(mTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniformMatrix4fv(mMVMatrixUniform, 1, GL_FALSE, &mMVMatrix[0][0]);
    glUniformMatrix4fv(mMVPMatrixUniform, 1, GL_FALSE, &mMVPMatrix[0][0]);
    glUniformMatrix3fv(mNormalMatrixUniform, 1, GL_FALSE, &mNormalMatrix[0][0]);

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
        source = "#version 100\n"
                 "precision highp float;\n"
                 + source;
    }
    else
    {
        source = "#version 110\n"
                 "#define lowp\n"
                 "#define highp\n"
                 "#define mediump\n"
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
    float perspectiveFar = 1000;
    auto perspectiveMat = glm::perspective(perspectiveFov, mAspect, perspectiveNear, perspectiveFar);

    mMVMatrix = translateMat * rotationMat * scaleMat;
    mMVPMatrix = perspectiveMat * mMVMatrix;
    mNormalMatrix = glm::inverse(glm::transpose(glm::mat3(mMVMatrix)));
}
