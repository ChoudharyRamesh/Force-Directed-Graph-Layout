/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<graphLayout.h>
#include<view.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_animationSpeedSpinBox_valueChanged(int arg1);
    void on_nodeSizeSpinBox_valueChanged(int arg1);
    void on_EdgeSizeSpinBox_valueChanged(int arg1);
    void on_actionFit_In_View_triggered();
    void on_actionLoad_From_adjacency_Matrix_triggered();
    void on_dragSpinBox_valueChanged(int arg1);
    void on_maxSpeedSpinBox_valueChanged(int arg1);
    void on_actionWith_Bracket_triggered();
    void on_actionWithout_Bracket_triggered();
    void on_mechanicalForceSlider_valueChanged(int value);
    void on_electricalForceSlider_valueChanged(int value);
    void on_actionInsert_Node_triggered();
    void on_actionRemove_Node_triggered();
    void on_actionConnect_Node_triggered();
    void on_actionDisconnect_Node_triggered();
    void on_actionNo_Editing_triggered();
    void on_actionExport_Scene_triggered();
    void on_actionClear_Scene_triggered();
    void on_actionLoad_From_Incidence_Matrix_triggered();
    void on_actionWithout_Bracket_2_triggered();
    void on_actionWith_Bracket_2_triggered();
    void activateEditingMode(GraphLayout::EditingMode mode);
    void on_actionzoom_in_triggered();
    void on_actionzoom_out_triggered();
    void on_actionAbout_triggered();
    void on_actionDemo_triggered();

private:
    Ui::MainWindow *ui;
    Scene * scene;
    View * view;
    GraphLayout * graphLayout;
    int edgeWidthFactor=3;
    int nodeRadiusFactor=25;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};


#endif // MAINWINDOW_H
