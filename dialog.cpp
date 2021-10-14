/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void Dialog::setText(QString str)
{
    ui->plainTextEdit->setPlainText(str);
}

void Dialog::setNodeNameText(QString str)
{
    ui->plainTextEdit_2->setPlainText(str);
}

QString Dialog::getText()
{
    return ui->plainTextEdit->toPlainText();
}

void Dialog::setPlaceHolderText(QString text)
{
    ui->plainTextEdit->setPlaceholderText(text);
}

void Dialog::setPlaceHolderTextForNodeList(QString text)
{
    ui->plainTextEdit_2->setPlaceholderText(text);
}

void Dialog::setTitle(QString title)
{
    setWindowTitle(title);
}

QList<QString> Dialog::getNodeNameList()
{
    QString text = ui->plainTextEdit_2->toPlainText();
    QStringList list = text.split("\n");
    QList<QString> ls;
    for(int i=0;i<list.size();i++)
    {
        QString nodeName = list.at(i);
        nodeName = nodeName.trimmed();
        ls.append(nodeName);
    }
    if(ls.last().isEmpty())ls.removeLast();
    return  ls;
}

void Dialog::setReadOnly()
{
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit_2->setReadOnly(true);
}

Dialog::~Dialog()
{
    delete ui;
}
