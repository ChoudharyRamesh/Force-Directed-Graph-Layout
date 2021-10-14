/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "view.h"
#include<graphLayout.h>

View::~View(){}

View::View(QWidget *parent)
    : QGraphicsView(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void View::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode)
{
    QGraphicsView::fitInView(rect,aspectRatioMode);
    emit transformed();
}

void View::enableEditing(bool set)
{
    this->m_editing=set;
}

void View::setGraphLayout(GraphLayout *graphLaoyout)
{
    this->graphLaoyout=graphLaoyout;
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if(isMousPressed && event->modifiers()==Qt::KeyboardModifier::ControlModifier)
    {
        QPointF newPoint =  event->pos();
        QPointF dif =  lastpoint - newPoint;
        if(transform().m11()<1)
            dif =  QPointF(dif.x()/transform().m11(),dif.y()/transform().m22());
        QRectF sR = sceneRect();
        setSceneRect(sR.x()+dif.x(),sR.y()+dif.y(),sR.width(),sR.height());
        lastpoint = newPoint;
    }
    else QGraphicsView::mouseMoveEvent(event);
}

void View::mousePressEvent(QMouseEvent *event)
{
    if(m_editing)
    {
        graphLaoyout->editingEvent(mapToScene(event->pos()));
    }
    else
    {
        isMousPressed=true;
        lastpoint =  event->pos();
        QGraphicsView::mousePressEvent(event);
    }
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    isMousPressed=false;
    QGraphicsView::mouseReleaseEvent(event);
}


void View::scalingTime(qreal q) {
    Q_UNUSED(q);
    qreal factor = 1.0+ qreal(_numScheduledScalings)/300.0;
    scale(factor,factor);
}

void View::animFinished() {

    if (_numScheduledScalings > 0)  _numScheduledScalings--;
    else  _numScheduledScalings++;
    sender()->~QObject();
    emit transformed();

}

void View::scaleView(int numSteps)
{
    _numScheduledScalings += numSteps;
    if (_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
        _numScheduledScalings = numSteps;
    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);
    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    anim->start();
}

void View::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers()==Qt::KeyboardModifier::ControlModifier)
    {
        int numDegrees = event->angleDelta().y() / 8;
        int numSteps = numDegrees / 15;
        scaleView(numSteps);
    }
    else QGraphicsView::wheelEvent(event);
}



