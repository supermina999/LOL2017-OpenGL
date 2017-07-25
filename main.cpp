#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setStyle("fusion");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    format.setVersion(3, 0);
#else
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
#endif
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
