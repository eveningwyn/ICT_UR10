#include "debugdialog.h"
#include "ui_debugdialog.h"
#include "ict_ur10.h"
#include <QMessageBox>
#include "language.h"
#include "staticname.h"

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
    QMessageBox::StandardButton rb = QMessageBox::warning(this,tr("提示"),
                                                          tr("在线扫描请选择Yes，离线扫描请选择No，取消请选择Cancel."),
                                                          QMessageBox::Cancel|QMessageBox::No|QMessageBox::Yes);
    if(rb==QMessageBox::Yes)
    {
        ptr->manualStartScan(true);
    }
    else
    {
        if(rb==QMessageBox::No)
        {
            ptr->manualStartScan(false);
        }
    }
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
    emit ict_start_test();
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
    emit debug_CylinderUpDown(CONTROL_OUT1_ON);
}

void DebugDialog::on_pushButton_cylinder_down_clicked()
{
    //气缸下降
    emit debug_CylinderUpDown(CONTROL_OUT1_OFF);
}

void DebugDialog::on_pushButton_return_clicked()
{
    emit returnSafePos();
}
