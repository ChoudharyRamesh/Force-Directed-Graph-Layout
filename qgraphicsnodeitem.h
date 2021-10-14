/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef QGRAPHICSNODEITEM_H
#define QGRAPHICSNODEITEM_H
#include<QGraphicsObject>
#include<QPainter>
#include<QVector2D>

class QGraphicsEdgeItem;
class Spring;

class QGraphicsNodeItem:public QGraphicsObject
{
    friend QGraphicsEdgeItem;
public:
    QGraphicsNodeItem(QGraphicsScene * scene, QPointF pos, int radius,
                      int NodeIndex, float * drag, int * maxSpeed,
                      QGraphicsItem *parent = nullptr, QString nodeName=QString());
    virtual ~QGraphicsNodeItem();
    void connectEdgeWith(QGraphicsNodeItem * node2,int edgePenWidth,bool directed=true);
    void disconnectEdgeWith(QGraphicsNodeItem * node2);
    bool hasEdgeWith(QGraphicsNodeItem * node2);
    bool hasDirectedEdgeWith(QGraphicsNodeItem * node2);
    void connectMechanicalSpringWith(QGraphicsNodeItem * node2,float * restingLength);
    void connectElectricalSpringWith(QGraphicsNodeItem * node2,  int *charge);
    void disconnectSpringWith(QGraphicsNodeItem * node2);
    void setPen(QPen pen);
    QPen & pen();
    void setNodeRadius(uint radius);
    void setPenWidthOfConnectedEdges(uint width);
    void accelerate(QVector2D acc);
    void balanceSprings();
    void updatePosition();
    QString getNodeName();

   //interface
public:
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QMap<QGraphicsNodeItem *,QPair<QGraphicsEdgeItem *,int>> connectedEdges;//connected with(node2),edge
    QMap<QGraphicsNodeItem *,Spring *> connectedSprings; // node2 , spring in between
    int nodeIndex;
    QPen m_pen;
    QColor nodeColor;
    QFont font;
    QRectF nodeRect;
    QVector2D acceleration=QVector2D(0,0);
    QVector2D velocity=QVector2D(0,0);
    int * maxSpeed;
    float * drag;
    QGraphicsScene * m_scene;
    void updateConnectedEdgePositons();
    int m_zValue=2;
    QString m_nodeName;

    // QGraphicsItem interface
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};

#endif // QGRAPHICSNODEITEM_H
