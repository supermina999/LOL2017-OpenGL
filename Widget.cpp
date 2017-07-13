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
    aspect = 1;
    position = {0, 0, -5};
    scale = {1, 1, 1};
    rotation = {0, 0, 0};
    rotationSpeed = 1;
    updateTransform();

    /*
    vertexes = {
        {1, 1, 1}, {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1},
        {1, 1, -1}, {1, -1, -1}, {-1, -1, -1}, {-1, 1, -1},
        {1, 1, 1}, {1, 1, -1}, {-1, 1, -1}, {-1, 1, 1},
        {1, -1, 1}, {-1, -1, 1}, {-1, -1, -1}, {1, -1, -1},
        {1, 1, 1}, {1, -1, 1}, {1, -1, -1}, {1, 1, -1},
        {-1, 1, 1}, {-1, 1, -1}, {-1, -1, -1}, {-1, -1, 1},
    };
    texCoords = {
        {1, 1}, {0, 1}, {0, 0}, {1, 0},
        {1, 1}, {1, 0}, {0, 0}, {0, 1},
        {1, 1}, {0, 1}, {0, 0}, {1, 0},
        {1, 1}, {1, 0}, {0, 0}, {0, 1},
        {1, 1}, {0, 1}, {0, 0}, {1, 0},
        {1, 1}, {1, 0}, {0, 0}, {0, 1},
    };
    */

    vertexes = {
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
    texCoords = {
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

    auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1000 / 60);
}

Widget::~Widget()
{
    makeCurrent();

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);

    glDeleteBuffers(1, &vertexArrayBuffer);
    glDeleteBuffers(1, &texCoordsArrayBuffer);
    glDeleteTextures(1, &texture);

    doneCurrent();
}

void Widget::onTimer()
{
    rotation.x += 0.015 * rotationSpeed;
    rotation.y += 0.005 * rotationSpeed;
    rotation.z += 0.005 * rotationSpeed;
    //scale.x += 0.001;
    updateTransform();
    update();
}

void Widget::slower()
{
    rotationSpeed *= 0.8;
}

void Widget::faster()
{
    rotationSpeed *= 1.2;
}

void Widget::initializeGL()
{
    glViewport(-1, -1, 2, 2);

    glClearColor(0, 0, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    program = glCreateProgram();
    vertexShader = createShader(":/Resources/vertex.glsl", GL_VERTEX_SHADER);
    fragmentShader = createShader(":/Resources/fragment.glsl", GL_FRAGMENT_SHADER);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glFinish();
    transformUniform = glGetUniformLocation(program, "transform");
    vertexAttrib = glGetAttribLocation(program, "position");
    texCoordAttrib = glGetAttribLocation(program, "texCoord");

    //glGenVertexArrays(1, &vertexArrayObject);
    //glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(vertexes[0]), vertexes.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texCoordsArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), texCoords.data(), GL_STATIC_DRAW);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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
    aspect = (float(w)) / h;
    updateTransform();
}

void Widget::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
    glEnableVertexAttribArray(vertexAttrib);
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, texCoordsArrayBuffer);
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniformMatrix4fv(transformUniform, 1, GL_FALSE, &transform[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, vertexes.size());

    doneCurrent();
}

GLuint Widget::createShader(QString filename, GLenum shaderType)
{
    auto shader = glCreateShader(shaderType);
    QFile sourceFile(filename);
    sourceFile.open(QIODevice::ReadOnly);
    auto source = QTextStream(&sourceFile).readAll();

    if(context()->isOpenGLES())
    {
        source = "#version 100\n" + source;
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
    auto translateMat = glm::translate(position);
    auto scaleMat = glm::scale(scale);
    auto rotateMatX = glm::rotate(rotation.x, glm::vec3(1, 0, 0));
    auto rotateMatY = glm::rotate(rotation.y, glm::vec3(0, 1, 0));
    auto rotateMatZ = glm::rotate(rotation.z, glm::vec3(0, 0, 1));
    auto rotationMat = rotateMatZ * rotateMatY * rotateMatX;

    float fov = 70;
    float near = 0.01;
    float far = 1000;
    auto perspectiveMat = glm::perspective(fov, aspect, near, far);

    transform = perspectiveMat * translateMat * rotationMat * scaleMat;
}
