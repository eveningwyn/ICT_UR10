#include "debuglogindialog.h"
#include "ui_debuglogindialog.h"
#include "staticname.h"
#include <QMessageBox>
#include "language.h"

DebugLoginDialog::DebugLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugLoginDialog)
{
    ui->setupUi(this);
}

DebugLoginDialog::~DebugLoginDialog()
{
    delete ui;
}

void DebugLoginDialog::on_pushButton_Login_clicked()
{
    if(USER_ID==ui->lineEdit_UserID->text().trimmed() && PASSWORD==ui->lineEdit_PWD->text())
    {
        ui->lineEdit_UserID->clear();
        ui->lineEdit_PWD->clear();
        ui->lineEdit_UserID->setFocus();
        accept();
    }
    else
    {
        QMessageBox::warning(this,tr("错误"),tr("用户名或者密码输入错误！\n"),QMessageBox::Ok);
        ui->lineEdit_UserID->clear();
        ui->lineEdit_PWD->clear();
        ui->lineEdit_UserID->setFocus();
    }
}

void DebugLoginDialog::on_pushButton_Cancel_clicked()
{
    ui->lineEdit_UserID->clear();
    ui->lineEdit_PWD->clear();
    ui->lineEdit_UserID->setFocus();
    close();
}
