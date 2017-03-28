#include "ict_mes_dialog.h"
#include "ui_ict_mes_dialog.h"
#include "staticname.h"
#include <QMessageBox>
#include "language.h"
#include <QSettings>
#include "ict_ur10.h"

ICT_MES_Dialog::ICT_MES_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ICT_MES_Dialog)
{
    ui->setupUi(this);
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString driveName = configRead->value(ICT_LOCAL_DRIVE).toString();
    QString sn_file_name = configRead->value(ICT_LOCAL_SN_FILE_NAME).toString();
    QString sn_name = configRead->value(ICT_LOCAL_SN_NAME).toString();
    QString request_file_name = configRead->value(ICT_LOCAL_RECEIVE_FILE_NAME).toString();
    QString request_name = configRead->value(ICT_LOCAL_RECEIVE_NAME).toString();
    QString run_file_name = configRead->value(ICT_LOCAL_RUN_FILE_NAME).toString();
    QString run_name = configRead->value(ICT_LOCAL_RUN_NAME).toString();
    QString result_file_name = configRead->value(ICT_LOCAL_RESULT_FILE_NAME).toString();
    QString result_name = configRead->value(ICT_LOCAL_RESULT_NAME).toString();
    QString yellow_limit = configRead->value(ICT_YELLOW_LIMIT).toString();
    QString red_limit = configRead->value(ICT_RED_LIMIT).toString();
    QString yield_base = configRead->value(ICT_YIELD_BASE).toString();
    QString yield_limit = configRead->value(ICT_YIELD_LIMIT).toString();
    QString test_timeout = configRead->value(ICT_TEST_TIMEOUT).toString();
    delete configRead;
    int flag = 0;
    if(""==driveName)
    {
        driveName = "Z";
        flag++;
    }
    if(""==sn_file_name)
    {
        sn_file_name = "UR";
        flag++;
    }
    if(""==sn_name)
    {
        sn_name = "SN.txt";
        flag++;
    }
    if(""==request_file_name)
    {
        request_file_name = "UR";
        flag++;
    }
    if(""==request_name)
    {
        request_name = "Request.txt";
        flag++;
    }
    if(""==run_file_name)
    {
        run_file_name = "UR";
        flag++;
    }
    if(""==run_name)
    {
        run_name = "RUN.txt";
        flag++;
    }
    if(""==result_file_name)
    {
        result_file_name = "UR";
        flag++;
    }
    if(""==result_name)
    {
        result_name = "Result.txt";
        flag++;
    }
    if(""==yellow_limit)
    {
        yellow_limit = "2";
        flag++;
    }
    if(""==red_limit)
    {
        red_limit = "3";
        flag++;
    }
    if(""==yield_base)
    {
        yield_base = "20";
        flag++;
    }
    if(""==yield_limit)
    {
        yield_limit = "0.95";
        flag++;
    }
    if(""==test_timeout)
    {
        test_timeout = "180";
        flag++;
    }
    ui->lineEdit_ICT_DriveName->setText(driveName);
    ui->lineEdit_SN_FileName->setText(sn_file_name);
    ui->lineEdit_SN_Name->setText(sn_name);
    ui->lineEdit_Receive_FileName->setText(request_file_name);
    ui->lineEdit_Receive_Name->setText(request_name);
    ui->lineEdit_Run_FileName->setText(run_file_name);
    ui->lineEdit_Run_Name->setText(run_name);
    ui->lineEdit_Result_FileName->setText(result_file_name);
    ui->lineEdit_Result_Name->setText(result_name);
    ui->lineEdit_yellow_limit->setText(yellow_limit);
    ui->lineEdit_Red_limit->setText(red_limit);
    ui->lineEdit_Yield_base->setText(yield_base);
    ui->lineEdit_Yield_limit->setText(yield_limit);
    ui->lineEdit_testTimeout->setText(test_timeout);
    if(0<flag)
    {
        saveICTConfig();
        QMessageBox::warning(this,tr("提示"),tr("ICT有部分参数已恢复默认设置，请检查ICT参数配置！\n"),QMessageBox::Ok);
    }
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    ui->checkBox_ICT_Enable->setChecked(ptr->ictEnable);
    on_checkBox_ICT_Enable_clicked();
}

ICT_MES_Dialog::~ICT_MES_Dialog()
{
    delete ui;
}

void ICT_MES_Dialog::on_pushButton_Save_ICT_Config_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,tr("保存ICT配置"),tr("是否保存当前ICT配置？\n非调试人员请勿动！"),QMessageBox::Yes|QMessageBox::No))
    {
        saveICTConfig();
        QMessageBox::warning(this,tr("保存ICT配置"),tr("保存ICT配置成功！\n"),QMessageBox::Ok);
    }
}

void ICT_MES_Dialog::on_checkBox_ICT_Enable_clicked()
{
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    if(!ui->checkBox_ICT_Enable->isChecked())
    {
        if(QMessageBox::Yes==QMessageBox::warning(this,tr("ICT"),tr("是否停止使用ICT？\n非调试人员请勿动！"),QMessageBox::Yes|QMessageBox::No))
        {
            ui->checkBox_ICT_Enable->setChecked(false);
        }
        else
        {
            ui->checkBox_ICT_Enable->setChecked(true);
        }
    }
    ptr->ictEnable = (ui->checkBox_ICT_Enable->isChecked());
    emit ptr->set_ict_Enable(ui->checkBox_ICT_Enable->isChecked());
}

void ICT_MES_Dialog::saveICTConfig()
{
    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    configWrite->setValue(ICT_LOCAL_IP, "10.10.6.5");
    configWrite->setValue(ICT_LOCAL_DRIVE, ui->lineEdit_ICT_DriveName->text());
    configWrite->setValue(ICT_LOCAL_SN_FILE_NAME, ui->lineEdit_SN_FileName->text());
    configWrite->setValue(ICT_LOCAL_SN_NAME, ui->lineEdit_SN_Name->text());
    configWrite->setValue(ICT_LOCAL_RECEIVE_FILE_NAME, ui->lineEdit_Receive_FileName->text());
    configWrite->setValue(ICT_LOCAL_RECEIVE_NAME, ui->lineEdit_Receive_Name->text());
    configWrite->setValue(ICT_LOCAL_RUN_FILE_NAME, ui->lineEdit_Run_FileName->text());
    configWrite->setValue(ICT_LOCAL_RUN_NAME, ui->lineEdit_Run_Name->text());
    configWrite->setValue(ICT_LOCAL_RESULT_FILE_NAME, ui->lineEdit_Result_FileName->text());
    configWrite->setValue(ICT_LOCAL_RESULT_NAME, ui->lineEdit_Result_Name->text());
    configWrite->setValue(ICT_YELLOW_LIMIT,ui->lineEdit_yellow_limit->text());
    configWrite->setValue(ICT_RED_LIMIT,ui->lineEdit_Red_limit->text());
    configWrite->setValue(ICT_YIELD_BASE,ui->lineEdit_Yield_base->text());
    configWrite->setValue(ICT_YIELD_LIMIT,ui->lineEdit_Yield_limit->text());
    configWrite->setValue(ICT_TEST_TIMEOUT,ui->lineEdit_testTimeout->text());
    delete configWrite;
}
