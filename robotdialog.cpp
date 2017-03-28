#include "robotdialog.h"
#include "ui_robotdialog.h"
#include "staticname.h"
#include <QMessageBox>
#include <QSettings>
#include "language.h"

RobotDialog::RobotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RobotDialog)
{
    ui->setupUi(this);
    initGetInfo();
}

RobotDialog::~RobotDialog()
{
    delete ui;
}

void RobotDialog::initGetInfo()
{
    isSave = true;
    ui->pushButtonDelType->setDisabled(false);

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);

    QString ipAddress   = configRead->value(SERVER_IP_ADDRESS).toString();
    QString port = configRead->value(SERVER_PORT).toString();
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotTypeEnable = configRead->value(ROBOT_TYPE_ENABLE).toString();
    int flag = 0;
    if(""==ipAddress)
    {
        ipAddress = "192.168.1.11";
        flag++;
    }
    if(""==port)
    {
        port = "12345";
        flag++;
    }
    if(""==robotIP)
    {
        robotIP = "192.168.1.1";
        flag++;
    }
    if(""==robotTypeEnable)
    {
        robotTypeEnable = "true";
        flag++;
    }
    ui->lineEditIPAddress->setText(ipAddress);
    ui->lineEditPort->setText(port);
    ui->lineEditRobotPAddress->setText(robotIP);
    if("true"==robotTypeEnable)
    {
        ui->checkBox_TypeEnable->setChecked(true);
    }
    else
    {
        if("false"==robotTypeEnable)
        {
            ui->checkBox_TypeEnable->setChecked(false);
        }
    }
    if(0<flag)
    {
        saveIPConfig();
        QMessageBox::warning(this,tr("提示"),tr("IP地址和端口号有部分参数已恢复默认设置，请检查参数配置！\n"),QMessageBox::Ok);
    }

    QString typeTemp = "";
    QString pro_numTemp = "";
    int rowCount = 0;
    for(int i=0; i<TYPE_TOTAL; i++)
    {
        typeTemp = configRead->value(QString(ROBOT_TYPE).arg(ui->comboBoxType->itemText(i))).toString();
        pro_numTemp = configRead->value(QString(ROBOT_PRO_NUM).arg(ui->comboBoxType->itemText(i))).toString();

        if(""!=typeTemp && ""!=pro_numTemp)
        {
            rowCount = ui->tableWidgetRobotType->rowCount();//获取当前总行数
            ui->tableWidgetRobotType->setRowCount(rowCount + 1);//插入一行
            rowCount = ui->tableWidgetRobotType->rowCount();//重新获取当前总行数
            int row = rowCount - 1;//获取最新行的行号
            ui->tableWidgetRobotType->setItem(row,0,new QTableWidgetItem(typeTemp));
            ui->tableWidgetRobotType->setItem(row,1,new QTableWidgetItem(pro_numTemp));
        }
    }
    delete configRead;
}

void RobotDialog::on_Setting_clicked()
{
    if(QMessageBox::Yes == QMessageBox::warning(this,tr("保存配置"),tr("确认保存配置参数？"),
                                                QMessageBox::Yes|QMessageBox::No))
    {
        saveIPConfig();
        QMessageBox::warning(this,tr("保存配置"),tr("保存配置成功！\n"),QMessageBox::Ok);
    }
}

void RobotDialog::on_pushButtonAddType_clicked()
{
    int rowCount = ui->tableWidgetRobotType->rowCount();//获取当前总行数
    if(rowCount > TYPE_TOTAL-1)
    {
        QMessageBox::warning(this,tr("错误信息"),QString(tr("添加错误！\n只允许添加%1套产品类型！")).arg(TYPE_TOTAL),QMessageBox::Ok);
        return;
    }
    QString typeSet = ui->comboBoxType->currentText();
    QString pro_numSet = ui->comboBoxPro_Num->currentText();
    QString typeTemp;
    QString pro_numTemp;
    for(int i=0;i<rowCount;i++)
    {
        typeTemp = ui->tableWidgetRobotType->item(i,0)->text();
        pro_numTemp = ui->tableWidgetRobotType->item(i,1)->text();
        if(typeSet == typeTemp || pro_numSet == pro_numTemp)
        {
            QMessageBox::warning(this,tr("错误信息"),tr("添加错误！\n这个参数在之前已经被添加！"),QMessageBox::Ok);
            return;
        }
    }

    if(QMessageBox::Yes == QMessageBox::warning(this,tr("新产品类型"),tr("请确认是否添加新的产品类型？"),
                                                QMessageBox::Yes|QMessageBox::No))
    {
        isSave = false;
        ui->pushButtonDelType->setDisabled(true);
        ui->tableWidgetRobotType->setRowCount(rowCount + 1);//插入一行
        rowCount = ui->tableWidgetRobotType->rowCount();//重新获取当前总行数
        int row = rowCount - 1;//获取最新行的行号
        ui->tableWidgetRobotType->setItem(row,0,new QTableWidgetItem(typeSet));
        ui->tableWidgetRobotType->setItem(row,1,new QTableWidgetItem(pro_numSet));
    }
}

void RobotDialog::on_pushButtonDelType_clicked()
{
    if(0 == ui->tableWidgetRobotType->rowCount())//获取当前总行数,并判断
    {
        return;
    }
    if(false==isSave)
    {
        QMessageBox::warning(this,tr("错误信息"),tr("参数没有保存，无法删除信息！"),QMessageBox::Ok);
        return;
    }

    int row = ui->tableWidgetRobotType->currentRow();//获取当前所选行数
    if(row != -1)
    {
        QString typeTemp = ui->tableWidgetRobotType->item(row,0)->text();
        QString pro_numTemp = ui->tableWidgetRobotType->item(row,1)->text();
        if(""==typeTemp || ""==pro_numTemp)
        {
            return;
        }
        if(QMessageBox::Yes == QMessageBox::warning(this,tr("删除"),tr("请确认是否删除这个产品类型？"),
                                                    QMessageBox::Yes|QMessageBox::No))
        {
            /**************删除本地配置文件的存储内容***************/

            QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
            configWrite->remove(QString(ROBOT_TYPE).arg(typeTemp));
            configWrite->remove(QString(ROBOT_PRO_NUM).arg(typeTemp));

            ui->tableWidgetRobotType->removeRow(row);
            ui->tableWidgetRobotType->setFocusPolicy(Qt::NoFocus);
            delete configWrite;

            saveConfig();//重新保存配置
            QMessageBox::warning(this,tr("删除"),tr("删除成功！"),QMessageBox::Ok);
        }
    }
}

void RobotDialog::on_pushButtonSaveType_clicked()
{
    if(0 == ui->tableWidgetRobotType->rowCount())//获取当前总行数,并判断
    {
        return;
    }

    if(QMessageBox::Yes == QMessageBox::warning(this,tr("保存类型"),tr("请确认是否保存产品类型？"),
                                                QMessageBox::Yes|QMessageBox::No))
    {
        saveConfig();//保存配置
        QMessageBox::warning(this,tr("保存类型"),tr("参数保存成功！"),QMessageBox::Ok);
    }
}

void RobotDialog::saveConfig()
{
    int rowCount = ui->tableWidgetRobotType->rowCount();//获取当前总行数
    QString typeTemp = "";
    QString pro_numTemp = "";

    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    for(int j=0;j<TYPE_TOTAL;j++)
    {
        configWrite->remove(QString(MAIN_UI_TYPE).arg(j));
    }
    for(int i=0; i<rowCount; i++)
    {
        typeTemp = ui->tableWidgetRobotType->item(i,0)->text();
        pro_numTemp = ui->tableWidgetRobotType->item(i,1)->text();
        configWrite->setValue(QString(ROBOT_TYPE).arg(typeTemp), typeTemp);
        configWrite->setValue(QString(ROBOT_PRO_NUM).arg(typeTemp), pro_numTemp);
        configWrite->setValue(QString(MAIN_UI_TYPE).arg(i), typeTemp);

        typeTemp = "";
        pro_numTemp = "";
    }
    delete configWrite;
    isSave = true;
    ui->pushButtonDelType->setDisabled(false);
}

void RobotDialog::saveIPConfig()
{
    QString ipAddress = ui->lineEditIPAddress->text();
    QString port = ui->lineEditPort->text();
    QString robotIP = ui->lineEditRobotPAddress->text();
    QString robotTypeEnable;
    if(ui->checkBox_TypeEnable->isChecked())
    {
        robotTypeEnable = "true";
    }
    else
    {
        robotTypeEnable = "false";
    }

    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    configWrite->setValue(SERVER_IP_ADDRESS, ipAddress);
    configWrite->setValue(SERVER_PORT, port);
    configWrite->setValue(ROBOT_IP, robotIP);
    configWrite->setValue(ROBOT_TYPE_ENABLE, robotTypeEnable);
    delete configWrite;
}

void RobotDialog::on_checkBox_TypeEnable_clicked()
{
    QString robotTypeEnable = "false";
    if(ui->checkBox_TypeEnable->isChecked())
    {
        robotTypeEnable = "true";
    }
    else
    {
        robotTypeEnable = "false";
    }
    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    configWrite->setValue(ROBOT_TYPE_ENABLE, robotTypeEnable);
    delete configWrite;
}
