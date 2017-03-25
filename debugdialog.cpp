#include "debugdialog.h"
#include "ui_debugdialog.h"
DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::on_pushButton_moveToScan_clicked()
{
    emit moveToScan();
}

void DebugDialog::on_pushButton_Scan_clicked()
{
    //扫描SN
}

void DebugDialog::on_pushButton_pickUp_Carrier_clicked()
{
    emit fixturePickup();
}

void DebugDialog::on_pushButton_place_Carrier_clicked()
{
    emit fixturePlace();
}

void DebugDialog::on_pushButton_place_ICT_clicked()
{
    emit ictPlace();
}

void DebugDialog::on_pushButton_pickUp_ICT_clicked()
{
    emit ictPickup();
}

void DebugDialog::on_pushButton_ICTClose_clicked()
{
    emit ictClose();
}

void DebugDialog::on_pushButton_ICTOpen_clicked()
{
    emit ictOpen();
}

void DebugDialog::on_pushButton_ICT_Run_clicked()
{
    //ict开始run
}

void DebugDialog::on_pushButton_place_OK_clicked()
{
    emit placeOKPos();
}

void DebugDialog::on_pushButton_place_NG_clicked()
{
    emit placeNGPos();
}

void DebugDialog::on_pushButton_cylinder_up_clicked()
{
    //气缸上升
}

void DebugDialog::on_pushButton_cylinder_down_clicked()
{
    //气缸下降
}
