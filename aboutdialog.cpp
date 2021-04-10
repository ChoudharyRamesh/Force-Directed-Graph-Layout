/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setFixedSize(520,280);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->iconLabel->setPixmap(QPixmap(":/Icon/AppIcon.png").scaled(140,140));
    setWindowTitle("About FDG Layout");
    ui->textLabel->setText("<!DOCTYPE html>\
                           <style>\
                           .styles {color: white;}</style>\
                           <h2 class=\"styles\">\
            Force Directed Graph Layout ( Open-Source)\
            </h2>\
            </p>\
            <p class=\"styles\">\
            Developed By : Ramesh Choudhary\
            </p>\
            <p class=\"styles\">\
            Developer contact :qbytecore@gmail.com\
            <p class=\"styles\">\
            SourceCode : <a href=\"https://github.com/ChoudharyRamesh/Force-Directed-Graph-Layout\">Github</a>\
            </p>\
            </p>\
            <p class=\"styles\">\
            Version : 1.0\
            </p>\
            <p class=\"styles\">\
            <a href=\"https://choudharyramesh.github.io/PrivacyPolicy/FDG-Layout-Privacy-Policy.html\">Privacy-Policy</a>\
            </p>\
            <p class=\"styles\">\
            This software use Qt under GNU LGPLV3. For more information about Qt see <a href=\"https://www.qt.io/\">here</a>.\
            </p>\
            </html>");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
