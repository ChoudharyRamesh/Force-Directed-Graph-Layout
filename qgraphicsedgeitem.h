/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef QGRAPHICSEDGEITEM_H
#define QGRAPHICSEDGEITEM_H
#include<QGraphicsSceneMouseEvent>
#include<QGraphicsObject>
#include<QtMath>
#include<QPainter>

class QGraphicsNodeItem;
class QGraphicsEdgeItem:public QGraphicsObject
{
public:
    QGraphicsEdgeItem(QGraphicsNodeItem * node1, QGraphicsNodeItem * node2,
    QGraphicsScene * scene,QGraphicsItem * parent=nullptr);
    void setArrow(bool set);
    QLineF edge();
    void setEdge(QLineF edge);
    void setPen(QPen pen);
    QPen & pen();
    void setEdgeSize(uint size);
    void detachAndDelete();
    QPair<QGraphicsNodeItem * ,QGraphicsNodeItem *> getArrowDirection();

    // QGraphicsItem interface
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget) override;
    QRectF boundingRect() const override;

private:

    QLineF m_edge;
    QPen m_pen;
    bool m_hasArrow=true;
    QGraphicsNodeItem * node1;
    QGraphicsNodeItem * node2;
    int m_zValue=1;
    QGraphicsScene * m_scene;
};

#endif // QGRAPHICSEDGEITEM_H
