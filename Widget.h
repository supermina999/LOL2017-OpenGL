#ifndef WIDGET_H
#define WIDGET_H

#include <QOpenGLWidget>

#include <vector>

#include <glm/glm.hpp>

class Widget : public QOpenGLWidget
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

    GLuint program, vertexShader, fragmentShader;
    GLuint vertexArrayObject;
    GLuint vertexArrayBuffer, texCoordsArrayBuffer;
    std::vector<glm::vec3> vertexes;
    std::vector<glm::vec2> texCoords;

    GLuint vertexAttrib, texCoordAttrib;
    GLuint texture;
    GLuint transformUniform;

    glm::vec3 position, scale, rotation;
    glm::mat4 transform;
    float aspect;

    float rotationSpeed;
};

#endif // WIDGET_H
