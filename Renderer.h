#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLExtraFunctions>

class Mesh;
class Camera;
class Light;

class Renderer : private QOpenGLExtraFunctions
{
public:
    Renderer();

    void initialize();

    void setShadowSize(int width, int height);
    void setScreenSize(int width, int height);

    void renderShadows(const std::vector<Mesh>& meshes, const Light& light);
    void render(const std::vector<Mesh>& meshes, const Camera& camera, const Light& light, GLuint defaultFramebufferObject);

    GLuint getProgram() const;
    GLuint getShadowProgram() const;

private:
    GLuint createShader(QString filename, GLenum shaderType);

    GLuint mProgram, mVertexShader, mFragmentShader;
    GLuint mShadowProgram, mShadowVertexShader, mShadowFragmentShader, mShadowFramebuffer, mShadowTexture;

    int mWidth = 1920;
    int mHeight = 1080;
    int mShadowWidth = 1920;
    int mShadowHeight = 1080;
};

#endif // RENDERER_H
