/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "qgraphicsedgeitem.h"
#include "qgraphicsnodeitem.h"
#include "scene.h"

QGraphicsEdgeItem::QGraphicsEdgeItem(QGraphicsNodeItem *node1,
    QGraphicsNodeItem *node2,QGraphicsScene * scene, QGraphicsItem * parent)
  :QGraphicsObject(parent)
{
    this->node1=node1;
    this->node2=node2;
    // arrow direction will be from node1 to node2
    setEdge(QLineF(node1->pos(),node2->pos()));
    setZValue(m_zValue);
    this->m_scene=scene;
}

void QGraphicsEdgeItem::setArrow(bool set)
{
    prepareGeometryChange();
    this->m_hasArrow=set;
}

QLineF QGraphicsEdgeItem::edge()
{
    return m_edge;
}

void QGraphicsEdgeItem::setEdge(QLineF edge)
{
    prepareGeometryChange();
    this->m_edge=edge;
}

void QGraphicsEdgeItem::setPen(QPen pen)
{
    prepareGeometryChange();
    this->m_pen=pen;
}

QPen & QGraphicsEdgeItem::pen()
{
    prepareGeometryChange();
    return  m_pen;
}

void QGraphicsEdgeItem::detachAndDelete()
{    
    node1->connectedEdges.remove(node2);
    node2->connectedEdges.remove(node1);
    m_scene->removeItem(this);
    this->deleteLater();
}

QPair<QGraphicsNodeItem *, QGraphicsNodeItem *> QGraphicsEdgeItem::getArrowDirection()
{
    if(!m_hasArrow) return  QPair<QGraphicsNodeItem*,QGraphicsNodeItem*>(nullptr,nullptr);
    else return  QPair<QGraphicsNodeItem*,QGraphicsNodeItem*>(node1,node2);
}

void QGraphicsEdgeItem::setEdgeSize(uint size)
{
    prepareGeometryChange();
    m_pen.setWidth(size);
}

QRectF QGraphicsEdgeItem::boundingRect() const
{
    int penWidth = 8*m_pen.width();
    QRectF rect = QRectF(m_edge.p1(),m_edge.p2()).normalized();
    if(m_hasArrow) rect+=QMarginsF(10+penWidth,10+penWidth,10+penWidth,10+penWidth);
    return rect;
}


void QGraphicsEdgeItem::paint(QPainter * painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(m_pen);
    painter->setBrush(m_pen.color());
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->drawLine(m_edge);

    if(m_hasArrow)
    {
        QLineF newLine = m_edge;
        float shift = m_pen.width();
        QPointF p1,p2,p3;
        p1=newLine.pointAt(0.5+((4*shift)/newLine.length()));
        QLineF normalLine = QLineF(newLine.center(),newLine.p2()).normalVector();
        p2=normalLine.pointAt((2*shift)/normalLine.length());
        p3=normalLine.pointAt(-(2*shift)/normalLine.length());
        QPolygonF polygon; polygon<<p1<<p2<<p3;
        painter->drawPolygon(polygon);
    }

}
