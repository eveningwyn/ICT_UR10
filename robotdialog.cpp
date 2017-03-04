#include "robotdialog.h"
#include "ui_robotdialog.h"
#include <QMessageBox>
#include <QSettings>
#include "staticname.h"

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
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    ui->lineEditIPAddress->setText(ipAddress);
    ui->lineEditPort->setText(port);
    ui->lineEditRobotPAddress->setText(robotIP);
    ui->lineEditRobotPort->setText(robotPort);

    QString typeTemp = "";
    QString pro_numTemp = "";
    int rowCount = 0;
    for(int i=0; i<COUNT; i++)
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
    if(QMessageBox::Yes == QMessageBox::warning(this,"Setting","Are you sure to configuration parameters?",
                                                QMessageBox::Yes|QMessageBox::No))
    {
        QString ipAddress = ui->lineEditIPAddress->text();
        QString port = ui->lineEditPort->text();
        QString robotIP = ui->lineEditRobotPAddress->text();
        QString robotPort = ui->lineEditRobotPort->text();

        QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        configWrite->setValue(SERVER_IP_ADDRESS, ipAddress);
        configWrite->setValue(SERVER_PORT, port);
        configWrite->setValue(ROBOT_IP, robotIP);
        configWrite->setValue(ROBOT_PORT, robotPort);
        delete configWrite;
    }
}

void RobotDialog::on_pushButtonAddType_clicked()
{
    int rowCount = ui->tableWidgetRobotType->rowCount();//获取当前总行数
    if(rowCount > COUNT)
    {
        QMessageBox::warning(this,"Error",QString("Allow only add %1 set of parameters!").arg(COUNT),QMessageBox::Ok);
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
            QMessageBox::warning(this,"Error","Add error!\nThis parameters have been added!",QMessageBox::Ok);
            return;
        }
    }

    if(QMessageBox::Yes == QMessageBox::warning(this,"New product type","Are you sure to add a new product type?",
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
        QMessageBox::warning(this,"Error","Parameter has not been saved!",QMessageBox::Ok);
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
        if(QMessageBox::Yes == QMessageBox::warning(this,"Delete","Are you sure to Delete this product type?",
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
            QMessageBox::warning(this,"Delete","Delete successed!",QMessageBox::Ok);
        }
    }
}

void RobotDialog::on_pushButtonSaveType_clicked()
{
    if(0 == ui->tableWidgetRobotType->rowCount())//获取当前总行数,并判断
    {
        return;
    }

    if(QMessageBox::Yes == QMessageBox::warning(this,"Save type","Are you sure to save product type?",
                                                QMessageBox::Yes|QMessageBox::No))
    {
        /**************保存配置到本地配置文件***************/
        saveConfig();//保存配置

        QMessageBox::warning(this,"Successed","Save successed!",QMessageBox::Ok);
    }
}

void RobotDialog::saveConfig()
{
    int rowCount = ui->tableWidgetRobotType->rowCount();//获取当前总行数
    QString typeTemp = "";
    QString pro_numTemp = "";

    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    for(int j=0;j<COUNT;j++)
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
