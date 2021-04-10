/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include <QApplication>
#include"mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(QPixmap(":/Icon/AppIcon.png")));
    w.show();
    return a.exec();
}
