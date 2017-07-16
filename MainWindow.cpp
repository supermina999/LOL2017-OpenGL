#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    resize(600, 600);
    setWindowTitle("LOL2017-OpenGL");
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    mGLWidget = new Widget();
    layout->addWidget(mGLWidget);

    auto buttonsLayout = new QHBoxLayout();
    int margin = fontMetrics().height() / 3;
    buttonsLayout->setContentsMargins(margin, margin, margin, margin);
    buttonsLayout->setSpacing(margin);
    layout->addLayout(buttonsLayout);

    auto slowerButton = new QPushButton("Slower");
    connect(slowerButton, SIGNAL(clicked()), mGLWidget, SLOT(slower()));
    buttonsLayout->addWidget(slowerButton);

    auto fasterButton = new QPushButton("Faster");
    connect(fasterButton, SIGNAL(clicked()), mGLWidget, SLOT(faster()));
    buttonsLayout->addWidget(fasterButton);

    setLayout(layout);
}
