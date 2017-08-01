#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLExtraFunctions>

class Texture : private QOpenGLFunctions
{
public:
    Texture(QString filename);
    ~Texture();

    GLuint getTextureId();

private:
    GLuint mTextureId;
};

#endif // TEXTURE_H
