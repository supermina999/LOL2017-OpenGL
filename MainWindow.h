#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "Widget.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    Widget* glWidget;
};

#endif // MAINWINDOW_H
