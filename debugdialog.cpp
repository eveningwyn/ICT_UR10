#include "debugdialog.h"
#include "ui_debugdialog.h"
#include "ict_ur10.h"

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
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    ptr->manualStartScan(true);
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

void DebugDialog::on_pushButton_return_clicked()
{
    emit returnSafePos();
}
