/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef GRAPH_LAYOUT_H
#define GRAPH_LAYOUT_H
#include<QRandomGenerator>
#include<QTimer>

class QGraphicsNodeItem;
class QGraphicsEdgeItem;
class View;
class Scene;
class GraphLayout:public QObject
{
    Q_OBJECT
public:
    enum EditingMode
    {
       NodeInsertion=Qt::UserRole+1,NodeRemoval,ConnectNode,DisconnectNode,NoEditing
    };
    static QList<QColor> colors;
    GraphLayout(const QList<QList<bool>> & adjacencyMatrix,
                Scene * scene,View * view ,QObject * parent=nullptr);

    void loadAdjacencyMatrix(const QList<QList<bool>> & adjacencyMatrix);
    void loadIncidenceMatrix(const QList<QList<int>> & incidenceMatrix);
    void loadAdjacencyMatrix(const QString & adjacencyMatrix);
    void loadIncidencMatrix(const QString & incidenceMatrix);
    QList<QList<bool>>getAdjacenyMatrix();
    QList<QList<int>> getIncidenceMatrix();
    QString getAdjacenyMatrix_AsString(bool withBracket=false);
    QString getIncidenceMatrix_AsString(bool withBracket=false);

    void insertNode(const QPointF& point);
    void removeNode(QGraphicsNodeItem* node);
    void connectNode(QGraphicsNodeItem * node1,QGraphicsNodeItem * node2);
    void disconnectNode(QGraphicsNodeItem * node1,QGraphicsNodeItem * node2);

    void clearLayout();
    void startAnimation();
    void stopAnimation();
    void setFrameInterval(int value);
    void setNodeRadius(uint size);
    void setEdgeSize(uint size);
    void setSpring_RestingLength(float value);
    void setDRAG(float value);
    void setMaxSpeed(int value);
    void setCharge( int value);

    void editingEvent(QPointF scenePos);
    void setEditingMode(EditingMode mode);

    void fitInView();
    void exportLayoutAsImage(QString filePath);

private:
    void balanceLayout();
    void grabAnimationStateAndPause();
    void resumeAnimationState();
    void moveNodesBy(QPointF point);
    bool hasEdge(int source,int destination) const;
    QList<QList<bool>> incidenceToAdjacentMat(const QList<QList<int>> & inciDenceMat);
    QList<QList<int>> adjacenceToIncidenceMat(const QList<QList<bool>> adjacenceMat );
    QList<QList<bool>> adjMat_StringToNumeric(const QString & adjMat);
    QList<QList<int>> inciMat_StringToNumeric(const QString & inciMat);

private:
    QList<QGraphicsNodeItem * > nodes;
    QList<QList<bool>> adjacencyMatrix;
    int nodeRadius;
    int edgePenSize;
    Scene * scene;
    View * view;
    QTimer timer;
    int colorCount=0;
    float spring_RestingLength=100;
    int charge = 5000;
    float DRAG=0.005;
    int maxSpeed=40;
    int currentNodeIndexCount=0;
    bool m_isAnimationRunning=false;
    bool is_AnimationPaused=false;
    EditingMode m_editingMode=NoEditing;
    QGraphicsNodeItem * editingNodeHolder=nullptr;
};
#endif // GRAPH_LAYOUT_H


















