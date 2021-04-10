/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(QObject *parent = nullptr);
};

#endif // SCENE_H
