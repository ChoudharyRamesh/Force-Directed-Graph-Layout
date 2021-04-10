/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "graphLayout.h"
#include "qgraphicsnodeitem.h"
#include "qgraphicsedgeitem.h"
#include "spring.h"
#include "view.h"
#include "scene.h"
#include<QDebug>
#include<QException>

#ifndef BoundedRAND
#define BounedRAND \
    ({ QPointF(QRandomGenerator::global()->bounded(-100,100),QRandomGenerator::global()->bounded(-100,100)); })
#endif

QList<QColor> GraphLayout::colors({"red","green","blue","yellow","purple","orange","#00ff7f",
                                   "#007fff","#96111e","#c71585", "#800000","#f032e6", "#e6194B","#aaffc3"});

GraphLayout::GraphLayout(const QList<QList<bool> > &adjacencyMatrix,
                         Scene *scene, View *view, QObject *parent):QObject(parent)
{
    this->scene=scene;
    this->view=view;
    timer.setInterval(10);
    this->nodeRadius=30;
    this->edgePenSize=3;
    QObject::connect(&timer,&QTimer::timeout,this,&GraphLayout::balanceLayout);
    view->setGraphLayout(this);
    loadAdjacencyMatrix(adjacencyMatrix);
}

void GraphLayout::clearLayout()
{
    this->adjacencyMatrix.clear();
    scene->clear();
    nodes.clear();
    colorCount=0;
    currentNodeIndexCount=1;
}


void GraphLayout::loadAdjacencyMatrix(const QList<QList<bool> > &adjacencyMatrix)
{
    setEditingMode(GraphLayout::NoEditing);
    clearLayout();
    this->adjacencyMatrix=adjacencyMatrix;

    //create nodes
    QPen pen;
    pen.setWidth(nodeRadius/6);
    for(int i=0;i<adjacencyMatrix.size();i++)
    {
        if(colorCount>=colors.size())colorCount=0;
        pen.setColor(colors.at(colorCount));
        colorCount++;

        QGraphicsNodeItem * node = new QGraphicsNodeItem(scene,BounedRAND,nodeRadius,currentNodeIndexCount++,&DRAG,&maxSpeed);
        node->setPen(pen);
        nodes.append(node);
    }

    for(int toDown=0;toDown<adjacencyMatrix.size();toDown++)
        for(int toLeft=toDown+1;toLeft<adjacencyMatrix.size();toLeft++)
        {
            bool v12 = hasEdge(toDown,toLeft);
            bool v21 = hasEdge(toLeft,toDown);

            if(v12 || v21)
            {
                if(v12 && !v21)
                {
                    nodes.at(toDown)->connectEdgeWith(nodes.at(toLeft),edgePenSize);
                }
                else if(!v12 && v21)
                {
                    nodes.at(toLeft)->connectEdgeWith(nodes.at(toDown),edgePenSize);
                }
                else nodes.at(toDown)->connectEdgeWith(nodes.at(toLeft),edgePenSize,false);
                nodes.at(toDown)->connectMechanicalSpringWith(nodes.at(toLeft),&spring_RestingLength);
            }
            else nodes.at(toDown)->connectElectricalSpringWith(nodes.at(toLeft),&charge);
        }

    for(int i=0;i<1000;i++)balanceLayout();
    fitInView();
}

void GraphLayout::loadAdjacencyMatrix(const QString &adjacencyMatrix)
{
    if(adjacencyMatrix.isEmpty())return;
    loadAdjacencyMatrix(adjMat_StringToNumeric(adjacencyMatrix));
}

void GraphLayout::loadIncidenceMatrix(const QList<QList<int> > &incidenceMatrix)
{
    loadAdjacencyMatrix(incidenceToAdjacentMat(incidenceMatrix));
}

void GraphLayout::loadIncidencMatrix(const QString &incidenceMatrix)
{
    if(incidenceMatrix.isEmpty())return;
    loadAdjacencyMatrix(incidenceToAdjacentMat(inciMat_StringToNumeric(incidenceMatrix)));
}

void GraphLayout::insertNode(const QPointF  & point)
{
    QPen pen;
    if(colorCount>=colors.size())colorCount=0;
    pen.setColor(colors.at(colorCount));
    colorCount++;
    QGraphicsNodeItem * node = new QGraphicsNodeItem(scene,point,nodeRadius,currentNodeIndexCount++,&DRAG,&maxSpeed);
    pen.setWidth(nodeRadius/6);
    node->setPen(pen);
    for(auto prenode:nodes)
        prenode->connectElectricalSpringWith(node,&charge);
    nodes.append(node);
}

void GraphLayout::removeNode(QGraphicsNodeItem * node)
{
    grabAnimationStateAndPause();
    // remove node from node list
    nodes.removeOne(node);
    // remove all attached spring with this node in others node
    for(auto otherNode :nodes)
        otherNode->disconnectSpringWith(node);

    //remove node from scene
    scene->removeItem(node);
    delete node;
    resumeAnimationState();
}

void GraphLayout::connectNode(QGraphicsNodeItem *node1, QGraphicsNodeItem *node2)
{
    grabAnimationStateAndPause();
    if(!node1->hasEdgeWith(node2))
    {
        // disconnect repulsive(electrical) springs
        // don't know which node has spring pointer so call on both node
        node1->disconnectSpringWith(node2);
        node2->disconnectSpringWith(node1);
        // add mechanical spring
        node1->connectMechanicalSpringWith(node2,&spring_RestingLength);
    }
    node1->connectEdgeWith(node2,edgePenSize);
    resumeAnimationState();
}

void GraphLayout::disconnectNode(QGraphicsNodeItem * node1, QGraphicsNodeItem *node2)
{
    grabAnimationStateAndPause();
    if(node1->hasEdgeWith(node2))
    {
        node1->disconnectEdgeWith(node2);
        // disconnect mechanical springs
        // don't know which node has spring pointer so call on both node
        node1->disconnectSpringWith(node2);
        node2->disconnectSpringWith(node1);
        // add electrical spring
        node1->connectElectricalSpringWith(node2,&charge);
    }
    resumeAnimationState();
}

QList<QList<bool>> GraphLayout::getAdjacenyMatrix()
{
    QList<QList<bool>> adjacencyMatrix;
    for(int i=0;i<nodes.size();i++)
    {
        QList<bool> mat;
        for(int j=0;j<nodes.size();j++)
        {
            if(i==j)
            {
                mat.append(0);
            }
            else
            {
                if(nodes.at(i)->hasDirectedEdgeWith(nodes.at(j)))
                    mat.append(1);
                else mat.append(0);
            }
        }

        adjacencyMatrix.append(mat);
    }
    return adjacencyMatrix;
}

QList<QList<int>> GraphLayout::getIncidenceMatrix()
{
    return  adjacenceToIncidenceMat(getAdjacenyMatrix());
}

QString GraphLayout::getAdjacenyMatrix_AsString(bool withBracket)
{
    QString adjacencyMatrixString;
    QList<QList<bool>> adjacencyMatrix = getAdjacenyMatrix();
    for(int i=0;i<adjacencyMatrix.size();i++)
    {
        if(withBracket)adjacencyMatrixString.append("{");
        for(int j=0;j<adjacencyMatrix.size();j++)
        {
            adjacencyMatrixString.append(QString::number(adjacencyMatrix.at(i).at(j))+",");
        }
        adjacencyMatrixString.resize(adjacencyMatrixString.size()-1);
        if(withBracket)adjacencyMatrixString.append("}");
        adjacencyMatrixString.append("\n");
    }
    adjacencyMatrixString.resize(adjacencyMatrixString.size()-1);
    return adjacencyMatrixString;
}

QString GraphLayout::getIncidenceMatrix_AsString(bool withBracket)
{
    QString incidenceMatrixString;
    QList<QList<int>> incidenceMatrix = getIncidenceMatrix();
    for(int i=0;i<incidenceMatrix.size();i++)
    {
        if(withBracket)incidenceMatrixString.append("{");
        for(int j=0;j<incidenceMatrix.at(i).size();j++)
        {
            incidenceMatrixString.append(QString::number(incidenceMatrix.at(i).at(j))+",");
        }
        incidenceMatrixString.resize(incidenceMatrixString.size()-1);
        if(withBracket)incidenceMatrixString.append("}");
        incidenceMatrixString.append("\n");
    }
    incidenceMatrixString.resize(incidenceMatrixString.size()-1);
    return incidenceMatrixString;
}

void GraphLayout::setFrameInterval(int value)
{    
    timer.stop();
    m_isAnimationRunning=false;
    is_AnimationPaused=false;
    timer.setInterval(value);
    if(value!=0)
    {
        m_isAnimationRunning=true;
        timer.start();
    }
}

void GraphLayout::startAnimation()
{
    if(!is_AnimationPaused)
    {
        m_isAnimationRunning = true;
        timer.start();
    }
}

void GraphLayout::stopAnimation()
{
    if(!is_AnimationPaused)
    {
        m_isAnimationRunning = false;
        timer.stop();
    }
}

void GraphLayout::grabAnimationStateAndPause()
{
    if(!m_isAnimationRunning) return;
    else
    {
        stopAnimation();
        is_AnimationPaused=true;
    }
}

void GraphLayout::resumeAnimationState()
{
    if(is_AnimationPaused)
    {
        is_AnimationPaused=false;
        startAnimation();
    }
}

void GraphLayout::setNodeRadius(uint size)
{
    for(auto node:nodes)
        node->setNodeRadius(size);
    this->nodeRadius=size;
}

void GraphLayout::setEdgeSize(uint size)
{
    // WARNING : double times call on each node due to connectivity ,need changes
    for(auto node:nodes)
        node->setPenWidthOfConnectedEdges(size);
    this->edgePenSize=size;
}


void GraphLayout::setMaxSpeed(int value)
{
    maxSpeed = value;
}

void GraphLayout::moveNodesBy(QPointF point)
{
    for(auto node:nodes)
        node->setPos(node->pos()+point);
}

void GraphLayout::setDRAG(float value)
{
    DRAG = value;
}

void GraphLayout::balanceLayout()
{  
    for(auto node :nodes)
        node->balanceSprings();
}

bool GraphLayout::hasEdge(int source, int destination) const
{
    if(source==destination)return false;
    return adjacencyMatrix.at(source).at(destination);
}


void GraphLayout::setSpring_RestingLength(float value)
{
    spring_RestingLength = value;
}

void GraphLayout::setCharge( int value)
{
    charge = value;
}

void GraphLayout::setEditingMode(GraphLayout::EditingMode mode)
{
    this->m_editingMode=mode;

    switch (m_editingMode) {
    case NodeInsertion :
    {
        view->setCursor(QCursor(QPixmap(":/Cursors/AddCursor.png").scaled(26,26),2,10));
        break;
    }

    case NodeRemoval :
    {
        view->setCursor(QCursor(QPixmap(":/Cursors/RemoveCursor.png").scaled(26,26),2,10));
        break;
    }

    case ConnectNode :
    {
     view->setCursor(QCursor(QPixmap(":/Cursors/ConnectCursor.png").scaled(26,26),2,10));
        break;
    }

    case DisconnectNode :
    {
      view->setCursor(QCursor(QPixmap(":/Cursors/DisconnectCursor.png").scaled(26,26),2,10));
        break;
    }

    case NoEditing : view->setCursor(Qt::ArrowCursor); break;

    }

    if(m_editingMode==NoEditing)
        view->enableEditing(false);
    else view->enableEditing(true);
    if(editingNodeHolder!=nullptr)
    {
        editingNodeHolder->setNodeRadius(nodeRadius);
        editingNodeHolder=nullptr;
    }
}

void GraphLayout::fitInView()
{
    QPointF viewCenter = view->rect().center();
    QPointF viewCenterInSceneCord = view->mapToScene(viewCenter.toPoint());

    QRectF itemBoundingRect = scene->itemsBoundingRect()+QMarginsF(1000,1000,1000,1000);
    QPointF itemBoundingRectCenter = itemBoundingRect.center();

    //find difference between view center and item bounding rect center
    QPointF difference = viewCenterInSceneCord - itemBoundingRectCenter;

    //move all items(nodes) around viewCenterInSceneCord using difference point
    moveNodesBy(difference);

    //move itembounding rect center at view center
    itemBoundingRect.moveCenter(viewCenterInSceneCord);

    //apply this itembounding rect as new scene rect
    scene->setSceneRect(itemBoundingRect);

    //fit scene in view
    view->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
}

void GraphLayout::exportLayoutAsImage(QString filePath)
{
    grabAnimationStateAndPause();
    fitInView();
    QTimer::singleShot(500,[this,filePath]()
    {
        if(!filePath.isEmpty())
        {
            QPixmap pixmap = view->grab();
            pixmap.save(filePath,"PNG",100);
        }
        resumeAnimationState();
    });
}

void GraphLayout::editingEvent(QPointF scenePos)
{
    switch (m_editingMode) {

    case NodeInsertion :
        insertNode(scenePos);
        break;

    case NodeRemoval :
    {
        QGraphicsNodeItem * node  =dynamic_cast<QGraphicsNodeItem*>
                (scene->itemAt(scenePos,view->transform()));
        if(node!=nullptr)removeNode(node); break;
    }

    case ConnectNode :
    {
        if(editingNodeHolder==nullptr)
        {
            editingNodeHolder = dynamic_cast<QGraphicsNodeItem * >
                    (scene->itemAt(scenePos,view->transform()));
            if(editingNodeHolder!=nullptr)editingNodeHolder->setNodeRadius(nodeRadius/1.3);
        }
        else
        {
            QGraphicsNodeItem * node  = dynamic_cast<QGraphicsNodeItem * >
                    (scene->itemAt(scenePos,view->transform()));
            if(node!=nullptr && node!=editingNodeHolder)
                connectNode(editingNodeHolder,node);
            editingNodeHolder->setNodeRadius(nodeRadius);
            editingNodeHolder=nullptr;
        }
        break;
    }

    case DisconnectNode :
    {
        if(editingNodeHolder==nullptr)
        {
            editingNodeHolder = dynamic_cast<QGraphicsNodeItem * >
                    (scene->itemAt(scenePos,view->transform()));
            if(editingNodeHolder!=nullptr)
                editingNodeHolder->setNodeRadius(nodeRadius/1.3);
        }
        else
        {
            QGraphicsNodeItem * node  = dynamic_cast<QGraphicsNodeItem * >
                    (scene->itemAt(scenePos,view->transform()));
            if(node!=nullptr && node!=editingNodeHolder)
                disconnectNode(editingNodeHolder,node);
            editingNodeHolder->setNodeRadius(nodeRadius);
            editingNodeHolder=nullptr;
        }
        break;
    }
    case NoEditing: break;
    }
}


QList<QList<bool> > GraphLayout::incidenceToAdjacentMat(const QList<QList<int> > &inciDenceMat)
{
    QList<QList<int>> inciToAd_NodeOnly;

    QList<QList<bool>> inciToAd;
    QList<bool> temp;
    for(int i=0;i<inciDenceMat.size();i++)
        temp.append(0);
    for(int i=0;i<inciDenceMat.size();i++)
        inciToAd.append(temp);

    for(int i=0;i<inciDenceMat.size();i++)
    {
        QList<int> connectedEdgesIndex;
        for(int j=0;j<inciDenceMat.at(i).size();j++)
        {
            if(inciDenceMat.at(i).at(j)==1)
                connectedEdgesIndex.push_back(j);
        }

        //column traverse
        QList<int> connectedNodes;
        for(int x=0;x<connectedEdgesIndex.size();x++)
        {
            int columnIndex = connectedEdgesIndex.at(x);
            for(int y=0;y<inciDenceMat.size();y++)
            {
                if(i==y)continue;
                else
                {
                    int edge = inciDenceMat.at(y).at(columnIndex);
                    if(edge==1|| edge==-1)
                    { connectedNodes.push_back(y); break; }
                }
            }
        }
        inciToAd_NodeOnly.push_back(connectedNodes);
    }

    for(int i=0;i<inciToAd_NodeOnly.size();i++)
        for(int j=0;j<inciToAd_NodeOnly.at(i).size();j++)
            inciToAd[i][inciToAd_NodeOnly.at(i).at(j)]=true;

    return  inciToAd;
}

QList<QList<int>> GraphLayout::adjacenceToIncidenceMat(const QList<QList<bool> > adjacenceMat)
{
    QList<QPair<QPair<int,int>,int>> edges;

    for(int y=0;y<adjacenceMat.size();y++)
        for(int x=y+1;x<adjacenceMat.size();x++)
        {
            int p = adjacenceMat.at(y).at(x);
            int q = adjacenceMat.at(x).at(y);
            if(p && q)
                edges.append(QPair<QPair<int,int>,int>(QPair<int,int>(y,x),0));
            else if(p && !q)
                edges.append(QPair<QPair<int,int>,int>(QPair<int,int>(y,x),1));
            else if(!p && q)
                edges.append(QPair<QPair<int,int>,int>(QPair<int,int>(y,x),-1));
        }

    QList<int> temp;
    for(int i=0;i<edges.size();i++)temp.append(0);
    QList<QList<int>> inciMat;
    for(int i=0;i<adjacenceMat.size();i++)
        inciMat.append(temp);

    for(int i=0;i<edges.size();i++)
    {
        int p=edges.at(i).first.first;
        int q=edges.at(i).first.second;
        int dir = edges.at(i).second;

        if(dir==0)
        {
            inciMat[p][i]=1;
            inciMat[q][i]=1;
        }
        else if(dir==1)
        {
            inciMat[p][i]=1;
            inciMat[q][i]=-1;
        }
        else if(dir==-1)
        {
            inciMat[p][i]=-1;
            inciMat[q][i]=1;
        }
    }
    return  inciMat;
}

QList<QList<bool>> GraphLayout::adjMat_StringToNumeric(const QString & adjMat)
{
    if(!adjMat.isEmpty())
    {
        QList<QList<bool>> matrix;
        QStringList slist = adjMat.split("\n");

        try {
            for(int y=0;y<slist.size();y++)
            {
                QStringList nlist = slist.at(y).split(",");
                if(slist.size()!=nlist.size())return QList<QList<bool>>();
                QList<bool> mat;
                for(int x=0;x<slist.size();x++)
                {
                    mat.append(nlist.at(x).trimmed().toInt());
                }
                matrix.append(mat);
            }
        }
        catch (QException e) { qDebug()<<e.what(); return QList<QList<bool>>();}
        return  matrix;
    }else return QList<QList<bool>>();
}

QList<QList<int>> GraphLayout::inciMat_StringToNumeric(const QString & inciMat)
{
    if(!inciMat.isEmpty())
    {
        QList<QList<int>> matrix;
        QStringList slist = inciMat.split("\n");
        try {
            for(int y=0;y<slist.size();y++)
            {
                QStringList nlist = slist.at(y).split(",");
                QList<int> mat;
                for(int x=0;x<nlist.size();x++)
                {
                    mat.append(nlist.at(x).trimmed().toInt());
                }
                matrix.append(mat);
            }
        }
        catch (QException e) { qDebug()<<e.what(); return QList<QList<int>>();}
        return matrix;
    }
    else return  QList<QList<int>>();
}


