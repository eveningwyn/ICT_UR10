﻿#include "logindialog.h"
#include "ui_logindialog.h"
#include "staticname.h"
#include "ict_ur10.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButtonLogin_clicked()
{
    if(USER_ID==ui->lineEditUserID->text().trimmed() && PASSWORD==ui->lineEditPassword->text())
    {
        ((ICT_UR10*)parentWidget())->Enable();
        ui->lineEditUserID->clear();
        ui->lineEditPassword->clear();
        ui->lineEditUserID->setFocus();
        hide();
    }
}

void LoginDialog::on_pushButtonCancel_clicked()
{
    close();
}

void LoginDialog::closeEvent(QCloseEvent *event)
{
    ((ICT_UR10*)parentWidget())->loginDlgIsShow = false;
    ui->lineEditUserID->clear();
    ui->lineEditPassword->clear();
    ui->lineEditUserID->setFocus();
    event->accept();  //accept
}
