/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    void setText(QString);
    QString getText();
    void setPlaceHolderText(QString text);
    void setTitle(QString title);
    ~Dialog();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
