#include "debugdialog.h"
#include "ui_debugdialog.h"
#include "staticname.h"
#include "ict_ur10.h"
#include <QMessageBox>
#include "language.h"
#include <QMutex>

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

void DebugDialog::closeEvent(QCloseEvent *event)
{
    QMessageBox::warning(this,NULL,tr("UR机器人需要复位，请注意人员和设备安全！\n"),QMessageBox::Ok);
    event->accept();  //accept
}

void DebugDialog::on_pushButton_moveToScan_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
    emit moveToScan();
}

void DebugDialog::on_pushButton_Scan_clicked()
{
    QMessageBox::StandardButton rb = QMessageBox::warning(this,tr("提示"),
                                                          tr("在线扫描请选择Yes，离线扫描请选择No，取消请选择Cancel."),
                                                          QMessageBox::Cancel|QMessageBox::No|QMessageBox::Yes);
    if(rb==QMessageBox::Yes)
    {
        ((ICT_UR10*)parentWidget())->manualStartScan(true);
    }
    else
    {
        if(rb==QMessageBox::No)
        {
            ((ICT_UR10*)parentWidget())->manualStartScan(false);
        }
    }
}

void DebugDialog::on_pushButton_pickUp_Carrier_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
        emit fixturePickup();
}

void DebugDialog::on_pushButton_place_Carrier_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
        emit fixturePlace();
}

void DebugDialog::on_pushButton_place_ICT_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
        emit ictPlace();
}

void DebugDialog::on_pushButton_pickUp_ICT_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
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
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
        emit placeOKPos();
}

void DebugDialog::on_pushButton_place_NG_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
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
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("当前操作有危险,确认当前操作?"),QMessageBox::Yes|QMessageBox::No))
        emit returnSafePos();
}

void DebugDialog::on_pushButton_ClawOpen_clicked()
{
    emit clawOpen();
}

void DebugDialog::on_pushButton_ClawClose_clicked()
{
    emit clawClose();
}

void DebugDialog::urRunning()
{
    ui->pushButton_return->setDisabled(true);
    ui->pushButton_moveToScan->setDisabled(true);
//    ui->pushButton_Scan->setDisabled(true);
    ui->pushButton_pickUp_Carrier->setDisabled(true);
    ui->pushButton_place_Carrier->setDisabled(true);
    ui->pushButton_place_ICT->setDisabled(true);
    ui->pushButton_pickUp_ICT->setDisabled(true);
//    ui->pushButton_ICTClose->setDisabled(true);
//    ui->pushButton_ICTOpen->setDisabled(true);
    ui->pushButton_ICT_Run->setDisabled(true);
    ui->pushButton_place_OK->setDisabled(true);
    ui->pushButton_place_NG->setDisabled(true);
//    ui->pushButton_ClawOpen->setDisabled(true);
//    ui->pushButton_ClawOpen->setDisabled(true);
//    ui->pushButton_cylinder_up->setDisabled(true);
//    ui->pushButton_cylinder_down->setDisabled(true);
}

void DebugDialog::runDone()
{
    ui->pushButton_return->setDisabled(false);
    ui->pushButton_moveToScan->setDisabled(false);
    ui->pushButton_Scan->setDisabled(false);
    ui->pushButton_pickUp_Carrier->setDisabled(false);
    ui->pushButton_place_Carrier->setDisabled(false);
    ui->pushButton_place_ICT->setDisabled(false);
    ui->pushButton_pickUp_ICT->setDisabled(false);
    ui->pushButton_ICTClose->setDisabled(false);
    ui->pushButton_ICTOpen->setDisabled(false);
    ui->pushButton_ICT_Run->setDisabled(false);
    ui->pushButton_place_OK->setDisabled(false);
    ui->pushButton_place_NG->setDisabled(false);
    ui->pushButton_ClawOpen->setDisabled(false);
    ui->pushButton_ClawOpen->setDisabled(false);
    ui->pushButton_cylinder_up->setDisabled(false);
    ui->pushButton_cylinder_down->setDisabled(false);
}

void DebugDialog::on_pushButton_robot_start_clicked()
{
    emit ((ICT_UR10*)parentWidget())->ui_robot_start();
//    ui->pushButton_robot_start->setDisabled(true);
}

void DebugDialog::on_pushButton_robot_pause_clicked()
{
    emit ((ICT_UR10*)parentWidget())->ui_robot_pause();
//    ui->pushButton_robot_start->setDisabled(false);
}

void DebugDialog::on_pushButton_robot_stop_clicked()
{
    emit ((ICT_UR10*)parentWidget())->ui_robot_stop();
//    ui->pushButton_robot_start->setDisabled(false);
}

void DebugDialog::Debug_dashboard(int index, QString showStr)
{
    static QMutex DebugUI_mutex;
    DebugUI_mutex.lock();
    if(1==index)//更改启动按钮文字显示
    {
        ui->pushButton_robot_start->setText(showStr);
    }
    else
    {
        if(2==index)//更改暂停按钮文字显示
        {
            ui->pushButton_robot_pause->setText(showStr);
        }
        else
        {
            if(3==index)//更改停止按钮文字显示
            {
                ui->pushButton_robot_stop->setText(showStr);
            }
        }
    }
    DebugUI_mutex.unlock();
}
