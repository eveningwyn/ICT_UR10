#include "logindialog.h"
#include "ui_logindialog.h"
//#include <QPushButton>
#include "ict_ur10.h"
#include <QMessageBox>

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
    if("admin"==ui->lineEditUserID->text().trimmed() && "admin"==ui->lineEditPassword->text())
    {
        ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
        ptr->Enable();
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
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    ptr->loginDlgIsShow = false;
    ui->lineEditUserID->clear();
    ui->lineEditPassword->clear();
    ui->lineEditUserID->setFocus();
    event->accept();  //accept
}
