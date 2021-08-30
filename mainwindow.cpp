/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "dialog.h"
#include "scene.h"
#include<QFileDialog>

QList<QList<bool>> demoMat =
{{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
 {1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
 {1,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0},
 {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
 {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0},
 {0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
 {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
 {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(900,800);
    setWindowTitle("FDG Layout");

    scene = new Scene(this);
    view = new View(ui->centralwidget);
    ui->verticalLayout->addWidget(view);
    graphLayout = new GraphLayout(QList<QList<bool>>(),scene,view);

    scene->setBackgroundBrush(QBrush("black"));
    view->setScene(scene);
    ui->settingFrame->show();
    ui->menubar->addAction(ui->actionSetting);
    connect(ui->actionSetting,&QAction::triggered,this,[this](){
        if(ui->settingFrame->isVisible())ui->settingFrame->hide();
        else ui->settingFrame->show();
        graphLayout->fitInView();
    });


    ui->animationSpeedSpinBox->setRange(0,50);
    ui->animationSpeedSpinBox->setValue(49);

    ui->mechanicalForceSlider->setRange(0,6000);
    ui->mechanicalForceSlider->setValue(500);
    ui->electricalForceSlider->setRange(0,99999999);
    ui->electricalForceSlider->setValue(1353383);

    ui->nodeSizeSpinBox->setValue(20);
    ui->nodeSizeSpinBox->setRange(1,50);
    ui->EdgeSizeSpinBox->setValue(3);
    ui->EdgeSizeSpinBox->setRange(1,25);
    ui->dragSpinBox->setRange(0,1000);
    ui->dragSpinBox->setValue(4);
    ui->maxSpeedSpinBox->setRange(1,1000);
    ui->maxSpeedSpinBox->setValue(70);
    ui->actionNo_Editing->setChecked(true);

    connect(view,&View::transformed,this,[this](){
        QTransform t = view->transform();
        int edgeSize = (edgeWidthFactor/3)+edgeWidthFactor/(t.m11()*1.414);
        graphLayout->setEdgeSize(edgeSize);
        int nodeSize =  (nodeRadiusFactor/3)+nodeRadiusFactor/(t.m11()*1.414);
        graphLayout->setNodeRadius(nodeSize);
    });

    graphLayout->startAnimation();
}

MainWindow::~MainWindow()
{
    delete graphLayout;
    delete view;
    delete ui;
}

void MainWindow::loadFromAdjacencyMatrix(QString data)
{
    graphLayout->loadAdjacencyMatrix(data);
}

void MainWindow::on_actionzoom_in_triggered()
{
    view->scaleView(1);
}

void MainWindow::on_actionzoom_out_triggered()
{
    view->scaleView(-1);
}


void MainWindow::on_dragSpinBox_valueChanged(int arg1)
{
    graphLayout->setDRAG(arg1*0.001f);
}

void MainWindow::on_animationSpeedSpinBox_valueChanged(int arg1)
{
    if(arg1==0)graphLayout->setFrameInterval(0);
    else  graphLayout->setFrameInterval(1001-arg1*20);
}

void MainWindow::on_EdgeSizeSpinBox_valueChanged(int arg1)
{
    QTransform t = view->transform();
    edgeWidthFactor=arg1;
    int size = (edgeWidthFactor/3)+edgeWidthFactor/(t.m11()*1.414);
    graphLayout->setEdgeSize(size);
}

void MainWindow::on_nodeSizeSpinBox_valueChanged(int arg1)
{
    QTransform t = view->transform();
    nodeRadiusFactor=arg1;
    int size = (nodeRadiusFactor/3)+nodeRadiusFactor/(t.m11()*1.414);
    graphLayout->setNodeRadius(size);
}

void MainWindow::on_actionClear_Scene_triggered()
{
    graphLayout->clearLayout();
}

void MainWindow::on_mechanicalForceSlider_valueChanged(int value)
{
    graphLayout->setSpring_RestingLength(value);
    QTimer::singleShot(100,this,[this](){  graphLayout->fitInView(); });
}

void MainWindow::on_electricalForceSlider_valueChanged(int value)
{
    graphLayout->setCharge(value);
    QTimer::singleShot(100,this,[this](){  graphLayout->fitInView(); });
}

void MainWindow:: on_actionFit_In_View_triggered()
{
    graphLayout->fitInView();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if(event->size().width()>10 && event->size().height()>10)
        QTimer::singleShot(100,this,[this](){ graphLayout->fitInView(); });
}

void MainWindow::on_actionLoad_From_adjacency_Matrix_triggered()
{
    Dialog dialog(this);
    dialog.setTitle("Load Adjacency Matrix");
    dialog.setPlaceHolderText("Type/Paste your adjacency Matrix with comma separator\ne.g :- for three node\n1,0,1\n1,0,0\n0,1,0");
    dialog.exec();
    graphLayout->loadAdjacencyMatrix( dialog.getText());
}


void MainWindow::on_actionLoad_From_Incidence_Matrix_triggered()
{
    graphLayout->setEditingMode(GraphLayout::NoEditing);
    Dialog dialog(this);
    dialog.setTitle("Load Incidence Matrix");
    dialog.setPlaceHolderText("Type/Paste your Incidence Matrix with comma separator\ne.g :- for three node\n1,0,1\n-1,0,0\n0,1,1");
    dialog.exec();
    QString text = dialog.getText();
    graphLayout->loadIncidencMatrix(dialog.getText());

}

void MainWindow::on_actionWith_Bracket_triggered()
{
    Dialog dialog;
    dialog.setTitle("Adjacency Matrix");
    dialog.setText(graphLayout->getAdjacenyMatrix_AsString(true));
    dialog.exec();
}

void MainWindow::on_actionWithout_Bracket_triggered()
{
    Dialog dialog;
    dialog.setTitle("Adjacency Matrix");
    dialog.setText(graphLayout->getAdjacenyMatrix_AsString());
    dialog.exec();
}

void MainWindow::on_actionWith_Bracket_2_triggered()
{
    Dialog dialog;
    dialog.setTitle("Incidence Matrix");
    dialog.setText(graphLayout->getIncidenceMatrix_AsString(true));
    dialog.exec();
}


void MainWindow::on_actionWithout_Bracket_2_triggered()
{
    Dialog dialog;
    dialog.setTitle("Incidence Matrix");
    dialog.setText(graphLayout->getIncidenceMatrix_AsString());
    dialog.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionDemo_triggered()
{
    graphLayout->loadAdjacencyMatrix(demoMat);
}

void MainWindow::on_actionExport_Scene_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this,"save",QString()," *.png");
    graphLayout->exportLayoutAsImage(filePath);
}

void MainWindow::on_maxSpeedSpinBox_valueChanged(int arg1)
{
    graphLayout->setMaxSpeed(arg1);
}

void MainWindow::on_actionInsert_Node_triggered()
{
    activateEditingMode(GraphLayout::NodeInsertion);
}

void MainWindow::on_actionRemove_Node_triggered()
{
    activateEditingMode(GraphLayout::NodeRemoval);
}

void MainWindow::on_actionConnect_Node_triggered()
{
    activateEditingMode(GraphLayout::ConnectNode);
}

void MainWindow::on_actionDisconnect_Node_triggered()
{
    activateEditingMode(GraphLayout::DisconnectNode);
}

void MainWindow::on_actionNo_Editing_triggered()
{
    activateEditingMode(GraphLayout::NoEditing);
}

void MainWindow::activateEditingMode(GraphLayout::EditingMode mode)
{
    ui->actionInsert_Node->setChecked(false);
    ui->actionRemove_Node->setChecked(false);
    ui->actionConnect_Node->setChecked(false);
    ui->actionDisconnect_Node->setChecked(false);
    ui->actionNo_Editing->setChecked(false);
    graphLayout->setEditingMode(mode);
    switch (mode){
    case GraphLayout::NodeInsertion :ui->actionInsert_Node->setChecked(true); break;
    case GraphLayout::NodeRemoval :ui->actionRemove_Node->setChecked(true);break;
    case GraphLayout::ConnectNode :ui->actionConnect_Node->setChecked(true);break;
    case GraphLayout::DisconnectNode :ui->actionDisconnect_Node->setChecked(true);break;
    case GraphLayout::NoEditing:ui->actionNo_Editing->setChecked(true); break;
    }
}
