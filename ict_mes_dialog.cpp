#include "ict_mes_dialog.h"
#include "ui_ict_mes_dialog.h"
#include <QMessageBox>
#include "language.h"
#include <QSettings>
#include "staticname.h"
#include "ict_ur10.h"

ICT_MES_Dialog::ICT_MES_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ICT_MES_Dialog)
{
    ui->setupUi(this);
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);

    QString ict_ip_addr = configRead->value(ICT_LOCAL_IP).toString();
    QString sn_file_name = configRead->value(ICT_LOCAL_SN_FILE_NAME).toString();
    QString sn_name = configRead->value(ICT_LOCAL_SN_NAME).toString();
    QString result_file_name = configRead->value(ICT_LOCAL_RESULT_FILE_NAME).toString();
    QString result_name = configRead->value(ICT_LOCAL_RESULT_NAME).toString();
    QString open_file_name = configRead->value(ICT_LOCAL_OPEN_FILE_NAME).toString();
    QString open_name = configRead->value(ICT_LOCAL_OPEN_NAME).toString();
    QString close_file_name = configRead->value(ICT_LOCAL_CLOSE_FILE_NAME).toString();
    QString close_name = configRead->value(ICT_LOCAL_CLOSE_NAME).toString();
    QString error_file_name = configRead->value(ICT_LOCAL_ERROR_FILE_NAME).toString();
    QString error_name = configRead->value(ICT_LOCAL_ERROR_NAME).toString();

    delete configRead;

    ui->lineEdit_ICT_IP_Addr->setText(ict_ip_addr);
    ui->lineEdit_A_FileName->setText(sn_file_name);
    ui->lineEdit_SN_Name->setText(sn_name);
    ui->lineEdit_B_FileName->setText(result_file_name);
    ui->lineEdit_Result_Name->setText(result_name);
    ui->lineEdit_C_FileName->setText(open_file_name);
    ui->lineEdit_Open_Name->setText(open_name);
    ui->lineEdit_D_FileName->setText(close_file_name);
    ui->lineEdit_Close_Name->setText(close_name);
    ui->lineEdit_F_FileName->setText(error_file_name);
    ui->lineEdit_Error_Name->setText(error_name);
    on_checkBox_ICT_Enable_clicked();
    on_checkBox_MES_Enable_clicked();
}

ICT_MES_Dialog::~ICT_MES_Dialog()
{
    delete ui;
}

void ICT_MES_Dialog::on_pushButton_Save_ICT_Config_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,tr("保存ICT配置"),tr("是否保存当前ICT配置？\n非调试人员请勿动！"),QMessageBox::Yes|QMessageBox::No))
    {
        QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);

        configWrite->setValue(ICT_LOCAL_IP, ui->lineEdit_ICT_IP_Addr->text());
        configWrite->setValue(ICT_LOCAL_SN_FILE_NAME, ui->lineEdit_A_FileName->text());
        configWrite->setValue(ICT_LOCAL_SN_NAME, ui->lineEdit_SN_Name->text());
        configWrite->setValue(ICT_LOCAL_RESULT_FILE_NAME, ui->lineEdit_B_FileName->text());
        configWrite->setValue(ICT_LOCAL_RESULT_NAME, ui->lineEdit_Result_Name->text());
        configWrite->setValue(ICT_LOCAL_OPEN_FILE_NAME, ui->lineEdit_C_FileName->text());
        configWrite->setValue(ICT_LOCAL_OPEN_NAME, ui->lineEdit_Open_Name->text());
        configWrite->setValue(ICT_LOCAL_CLOSE_FILE_NAME, ui->lineEdit_D_FileName->text());
        configWrite->setValue(ICT_LOCAL_CLOSE_NAME, ui->lineEdit_Close_Name->text());
        configWrite->setValue(ICT_LOCAL_ERROR_FILE_NAME, ui->lineEdit_F_FileName->text());
        configWrite->setValue(ICT_LOCAL_ERROR_NAME, ui->lineEdit_Error_Name->text());

        delete configWrite;
        QMessageBox::warning(this,tr("保存ICT配置"),tr("保存ICT配置成功！\n"),QMessageBox::Yes|QMessageBox::No);
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
    emit ptr->set_ict_Enable(ui->checkBox_ICT_Enable->isChecked());
}

void ICT_MES_Dialog::on_checkBox_MES_Enable_clicked()
{
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    if(!ui->checkBox_MES_Enable->isChecked())
    {
        if(QMessageBox::Yes==QMessageBox::warning(this,tr("MES"),tr("是否停止使用MES？\n非调试人员请勿动！"),QMessageBox::Yes|QMessageBox::No))
        {
            ui->checkBox_MES_Enable->setChecked(false);
        }
        else
        {
            ui->checkBox_MES_Enable->setChecked(true);
        }
    }
    emit ptr->set_mes_Enable(ui->checkBox_MES_Enable->isChecked());
}

void ICT_MES_Dialog::on_pushButtonGetWebConfig_clicked()
{
    QString input = ui->textEdit_Input->toPlainText();
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    emit ptr->get_MES_Config(input);
}

void ICT_MES_Dialog::showConfig(QString config)
{
    ui->textBrowser_WEB_Output->clear();
    ui->textBrowser_WEB_Output->setPlainText(config);
}
