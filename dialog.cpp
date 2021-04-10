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

QString Dialog::getText()
{
    return ui->plainTextEdit->toPlainText();
}

void Dialog::setPlaceHolderText(QString text)
{
    ui->plainTextEdit->setPlaceholderText(text);
}

void Dialog::setTitle(QString title)
{
   setWindowTitle(title);
}

Dialog::~Dialog()
{
    delete ui;
}
