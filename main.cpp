/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include <QApplication>
#include"mainwindow.h"
//#include<QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.setWindowIcon(QIcon(QPixmap(":/Icon/AppIcon.png")));

    if(argc>1)
    {
        int row = QString(argv[1]).toInt();
        QString cmd_adjacencyMatrix;
        for(int i=0;i<row;i++)
        {
            cmd_adjacencyMatrix.append(argv[2+i]);
            cmd_adjacencyMatrix.append("\n");
        }
        cmd_adjacencyMatrix.remove(cmd_adjacencyMatrix.size()-2,2);
        w.loadFromAdjacencyMatrix(cmd_adjacencyMatrix);
           // qDebug()<<cmd_adjacencyMatrix;
    }


    w.show();
    return a.exec();
}
