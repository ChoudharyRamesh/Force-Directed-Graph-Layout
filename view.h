/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef VIEW_H
#define VIEW_H

#include<QGraphicsView>
#include<QTimeLine>
#include<QWheelEvent>
#include<QMouseEvent>
#include<QtMath>

class GraphLayout;
class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QWidget *parent = nullptr);
    ~View();
    void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio);
    void scaleView(int numSteps);
    void enableEditing(bool set);
    void setGraphLayout(GraphLayout * graphLaoyout);

signals:
    void transformed();

private:
    int _numScheduledScalings=0;
    bool isMousPressed=false;
    QPointF lastpoint;
    bool m_editing=false;
    GraphLayout * graphLaoyout=nullptr;

private slots:
    // smoothly zoom view , code taken from https://wiki.qt.io/Smooth_Zoom_In_QGraphicsView
    void scalingTime(qreal);
    void animFinished();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};


#endif // VIEW_H
