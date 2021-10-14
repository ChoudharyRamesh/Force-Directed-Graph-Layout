/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "qgraphicsnodeitem.h"
#include "qgraphicsedgeitem.h"
#include "spring.h"
#include "scene.h"
#include<QDebug>
#include<QGraphicsView>
QGraphicsNodeItem::QGraphicsNodeItem(QGraphicsScene * scene, QPointF pos, int radius, int NodeIndex, float *drag, int *maxSpeed, QGraphicsItem *parent,
                         QString nodeName):QGraphicsObject(parent)
{
    this->nodeIndex=NodeIndex;
    setPos(pos); // position of item in scene coordinate
    nodeRect = QRectF(0,0,0,0)+QMarginsF(radius,radius,radius,radius); // position of item in item coordinate
    font.setPixelSize(radius);
    m_pen.setWidth(radius/6);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    this->drag=drag;
    this->maxSpeed=maxSpeed;
    this->m_scene=scene;
    m_scene->addItem(this);
    setZValue(m_zValue);
    if(nodeName.isEmpty())m_nodeName = QString::number(NodeIndex);
    else m_nodeName=nodeName;
    setAcceptHoverEvents(true);
}


QGraphicsNodeItem::~QGraphicsNodeItem()
{
    //delete connected edges
    for(auto pair:connectedEdges.values())
        pair.first->detachAndDelete();
    qDeleteAll(connectedSprings.values());
}


QPainterPath QGraphicsNodeItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

QRectF QGraphicsNodeItem::boundingRect() const
{
    int margin = m_pen.width();
    return nodeRect+QMarginsF(margin,margin,margin,margin);
}

void QGraphicsNodeItem::connectEdgeWith(QGraphicsNodeItem * node2, int edgePenWidth, bool directed)
{
    // if this node doesn't  have edge with node2
    if(!hasEdgeWith(node2))
    {   // then add one
        QGraphicsEdgeItem * edge = new QGraphicsEdgeItem(this,node2,m_scene);
        if(directed)
            edge->pen().setColor(this->pen().color());
        else
        {
            edge->pen().setColor("white");
            edge->setArrow(false);
        }
        edge->pen().setWidth(edgePenWidth);
        this->connectedEdges.insert(node2,QPair<QGraphicsEdgeItem*,int>(edge,1));
        node2->connectedEdges.insert(this,QPair<QGraphicsEdgeItem*,int>(edge,2));
        m_scene->addItem(edge);
    }
    else if(node2->hasDirectedEdgeWith(this))
    {
        // if node2 have directed edge with node1 then turn into undirected(white)
        QGraphicsEdgeItem * edge = this->connectedEdges.find(node2).value().first;
        edge->pen().setColor("white");
        edge->pen().setWidth(edgePenWidth);
        edge->setArrow(false);
    }
}

void QGraphicsNodeItem::disconnectEdgeWith(QGraphicsNodeItem * node2)
{ 
    connectedEdges.value(node2).first->detachAndDelete();
}

void QGraphicsNodeItem::connectMechanicalSpringWith(QGraphicsNodeItem *node2, float *restingLength)
{
    Spring * spring = new Spring(this,node2,restingLength);
    connectedSprings.insert(node2,spring);
}

void QGraphicsNodeItem::connectElectricalSpringWith(QGraphicsNodeItem *node2, int *charge)
{
    Spring * spring = new Spring(this,node2,charge);
    connectedSprings.insert(node2,spring);
}

void QGraphicsNodeItem::disconnectSpringWith(QGraphicsNodeItem *node2)
{
    delete this->connectedSprings.value(node2);
    this->connectedSprings.remove(node2);
}

bool QGraphicsNodeItem::hasEdgeWith(QGraphicsNodeItem *node2)
{
    if(connectedEdges.find(node2)==connectedEdges.end())return false;
    else return true;
}

bool QGraphicsNodeItem::hasDirectedEdgeWith(QGraphicsNodeItem *node2)
{
    QMap<QGraphicsNodeItem *,QPair<QGraphicsEdgeItem *,int>>::iterator it = connectedEdges.find(node2);
    if(it==connectedEdges.end()) return false;
    else
    {
        QGraphicsEdgeItem * edge = it->first;
        if(edge->pen().color()=="white")return  true;
        else
        {
            if(edge->getArrowDirection().first==this)
                return true;
            else return false;
        }
    }
}

void QGraphicsNodeItem::setPen(QPen pen)
{
    prepareGeometryChange();
    this->m_pen=pen;
}

QPen &QGraphicsNodeItem::pen()
{
    prepareGeometryChange();
    return m_pen;
}

void QGraphicsNodeItem::setNodeRadius(uint radius)
{
    prepareGeometryChange();
    nodeRect = QRectF(0,0,0,0)+QMarginsF(radius,radius,radius,radius);
    m_pen.setWidth(radius/6);
    font.setPixelSize(radius);
}

void QGraphicsNodeItem::setPenWidthOfConnectedEdges(uint width)
{
    for(auto pair:connectedEdges.values())
        pair.first->pen().setWidth(width);
}

void QGraphicsNodeItem::accelerate(QVector2D acc)
{
    acceleration+=acc;
}

void QGraphicsNodeItem::balanceSprings()
{
    for(auto spring:connectedSprings.values())
        spring->applyForce();
}

void QGraphicsNodeItem::updatePosition()
{
    prepareGeometryChange();
    velocity+=acceleration;
    velocity = velocity - velocity*(*drag); // reduce velocity
    if(velocity.length() > *maxSpeed)
    {
        velocity.normalize();
        velocity *= *maxSpeed;
    }
    QPointF pos = this->pos();
    pos+=velocity.toPointF();
    setPos(pos);
    acceleration=QVector2D(0,0);
}

QString QGraphicsNodeItem::getNodeName()
{
    return m_nodeName;
}


void QGraphicsNodeItem::updateConnectedEdgePositons()
{
    QGraphicsEdgeItem  * currentEdge;
    QPointF newPoint = scenePos();
    for(auto it = connectedEdges.begin();it!=connectedEdges.end(); it++)
    {
        currentEdge = it.value().first;
        if(it.value().second==1)currentEdge->setEdge(QLineF(newPoint,currentEdge->edge().p2()));
        else currentEdge->setEdge(QLineF(currentEdge->edge().p1(),newPoint));
    }
}

void QGraphicsNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setPen(m_pen);
    painter->setFont(font);
    painter->setBrush(QBrush("white"));
    painter->drawEllipse(nodeRect);
    QPen p = m_pen;
    p.setColor("black");
    painter->setPen(p);
    painter->drawText(nodeRect,Qt::AlignCenter,QString::number(nodeIndex));
    updateConnectedEdgePositons();
}

void QGraphicsNodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    scene()->views().first()->setToolTip(m_nodeName);
    QGraphicsItem::hoverEnterEvent(event);
}

void QGraphicsNodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    scene()->views().first()->setToolTip("");
    QGraphicsItem::hoverLeaveEvent(event);
}

