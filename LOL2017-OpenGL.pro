#-------------------------------------------------
#
# Project created by QtCreator 2017-06-24T10:12:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LOL2017-OpenGL
TEMPLATE = app

#android {
#  DEFINES += QT_OPENGL_ES_3
#  QMAKE_LFLAGS += -lGLESv3
#}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    thirdParty/glm

SOURCES += \
        main.cpp \
        Widget.cpp \
    MainWindow.cpp \
    Mesh.cpp \
    Light.cpp \
    Camera.cpp \
    Transform.cpp

HEADERS += \
        Widget.h \
    MainWindow.h \
    Mesh.h \
    Light.h \
    Camera.h \
    Transform.h

RESOURCES += \
    resources.qrc
