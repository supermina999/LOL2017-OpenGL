#include "MainWindow.h"
#include <QApplication>
#include <QOpenGLContext>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle("fusion");

    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setVersion(3, 3);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    else
    {
        format.setVersion(3, 0);
    }
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();

    return a.exec();
}
