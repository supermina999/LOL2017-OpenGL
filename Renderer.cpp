#include "Renderer.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QOpenGLContext>

#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

#ifdef Q_OS_ANDROID
#define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
#define GL_TEXTURE_COMPARE_MODE GL_TEXTURE_COMPARE_MODE_EXT
#define GL_COMPARE_REF_TO_TEXTURE GL_COMPARE_REF_TO_TEXTURE_EXT
#define GL_TEXTURE_COMPARE_FUNC GL_TEXTURE_COMPARE_FUNC_EXT
#endif

Renderer::Renderer(glm::vec3 clearColor, int shadowWidth, int shadowHeight)
{
    mShadowWidth = shadowWidth;
    mShadowHeight = shadowHeight;

    initializeOpenGLFunctions();

    glClearColor(clearColor.x, clearColor.y, clearColor.z, 1);
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
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowWidth, shadowHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowTexture, 0);
}

Renderer::~Renderer()
{
    glDeleteProgram(mProgram);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragmentShader);
    glDeleteProgram(mShadowProgram);
    glDeleteShader(mShadowVertexShader);
    glDeleteShader(mShadowFragmentShader);
    glDeleteFramebuffers(1, &mShadowFramebuffer);
    glDeleteTextures(1, &mShadowTexture);
}

void Renderer::setShadowSize(int width, int height)
{
    mShadowWidth = width;
    mShadowHeight = height;
}

void Renderer::setScreenSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void Renderer::renderShadows(const std::vector<std::shared_ptr<Mesh>>& meshes, std::shared_ptr<Light> light)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mShadowFramebuffer);
    glViewport(0, 0, mShadowWidth, mShadowHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2, 1);
    glCullFace(GL_FRONT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glUseProgram(mShadowProgram);

    for(auto& mesh : meshes)
    {
        auto lightMVPMatrix = light->getViewProjectionMatrix() * mesh->getModelMatrix();
        auto lightMVPMatrixUniform = glGetUniformLocation(mShadowProgram, "mvpMatrix");
        glUniformMatrix4fv(lightMVPMatrixUniform, 1, GL_FALSE, &lightMVPMatrix[0][0]);

        glBindVertexArray(mesh->getShadowVertexArrayObject());
        glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());
    }

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void Renderer::render(const std::vector<std::shared_ptr<Mesh>> &meshes, std::shared_ptr<Camera> camera, std::shared_ptr<Light> light, GLuint defaultFramebufferObject)
{
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject);
    glViewport(0, 0, mWidth, mHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glUseProgram(mProgram);

    auto shadowSamplerUniform = glGetUniformLocation(mProgram, "shadowSampler");
    glUniform1i(shadowSamplerUniform, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mShadowTexture);

    auto lightPosition = light->getPosition();
    auto lightPositionUniform = glGetUniformLocation(mProgram, "lightPosition");
    glUniform3fv(lightPositionUniform, 1, &lightPosition[0]);

    for(auto& mesh : meshes)
    {
        auto samplerUniform = glGetUniformLocation(mProgram, "sampler");
        glUniform1i(samplerUniform, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->getTexture()->getTextureId());

        auto mvMatrix = camera->getViewMatrix() * mesh->getModelMatrix();
        auto mvMatrixUniform = glGetUniformLocation(mProgram, "mvMatrix");
        glUniformMatrix4fv(mvMatrixUniform, 1, GL_FALSE, &mvMatrix[0][0]);

        auto mvpMatrix = camera->getProjectionMatrix() * mvMatrix;
        auto mvpMatrixUniform = glGetUniformLocation(mProgram, "mvpMatrix");
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, &mvpMatrix[0][0]);

        auto normalMatrix = glm::inverse(glm::transpose(glm::mat3(mvMatrix)));
        auto normalMatrixUniform = glGetUniformLocation(mProgram, "normalMatrix");
        glUniformMatrix3fv(normalMatrixUniform, 1, GL_FALSE, &normalMatrix[0][0]);

        glm::mat4 scaleBiasMatrix = {
            {0.5, 0, 0, 0},
            {0, 0.5, 0, 0},
            {0, 0, 0.5, 0},
            {0.5, 0.5, 0.5, 1}
        };
        auto lightMVPMatrix = light->getViewProjectionMatrix() * mesh->getModelMatrix();
        auto shadowMatrix = scaleBiasMatrix * lightMVPMatrix;
        auto shadowMatrixUniform = glGetUniformLocation(mProgram, "shadowMatrix");
        glUniformMatrix4fv(shadowMatrixUniform, 1, GL_FALSE, &shadowMatrix[0][0]);

        auto lightingParamsUniform = glGetUniformLocation(mProgram, "lightingParams");
        auto lightCoefs = light->getLightParams();
        auto matParams = mesh->getMaterialParams();
        glm::vec4 lightingParams = {matParams.x * lightCoefs.x, matParams.y * lightCoefs.y,
                                    matParams.z * lightCoefs.z, matParams.w};
        glUniform4fv(lightingParamsUniform, 1, &lightingParams[0]);

        glBindVertexArray(mesh->getVertexArrayObject());
        glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());
    }
}

GLuint Renderer::getProgram() const
{
    return mProgram;
}

GLuint Renderer::getShadowProgram() const
{
    return mShadowProgram;
}

GLuint Renderer::createShader(QString filename, GLenum shaderType)
{
    auto shader = glCreateShader(shaderType);
    QFile sourceFile(filename);
    sourceFile.open(QIODevice::ReadOnly);
    auto source = QTextStream(&sourceFile).readAll();

    if(QOpenGLContext::currentContext()->isOpenGLES())
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
        QMessageBox::warning(nullptr, "Can't compile shader", source);
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            auto infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, 0, infoLog);
            QString str(infoLog);
            QMessageBox::warning(nullptr, "Can't compile shader", str);

            delete[] infoLog;
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
