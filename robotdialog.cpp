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
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString ipAddress   = configRead->value(SERVER_IP_ADDRESS).toString();
    QString port = configRead->value(SERVER_PORT).toString();
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;

    ui->lineEditIPAddress->setText(ipAddress);
    ui->lineEditPort->setText(port);
    ui->lineEditRobotPAddress->setText(robotIP);
    ui->lineEditRobotPort->setText(robotPort);
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
    if(QMessageBox::Yes == QMessageBox::warning(this,"Warning","Are you sure to add a new product type?",
                                                QMessageBox::Yes|QMessageBox::No))
    {
        int rowCount = ui->tableWidgetRobotType->rowCount();//获取当前行数
        ui->tableWidgetRobotType->setRowCount(rowCount + 1);//插入一行
        rowCount = ui->tableWidgetRobotType->rowCount();//重新获取当前行数
        int row = ui->tableWidgetRobotType->rowCount() - 1;//获取最新行
        ui->tableWidgetRobotType->setItem(row,0,new QTableWidgetItem(QString("Type%1").arg(rowCount)));
        ui->tableWidgetRobotType->setItem(row,1,new QTableWidgetItem(QString("Program%1").arg(rowCount)));
    }
}

void RobotDialog::on_pushButtonDelType_clicked()
{
    int rowCount = ui->tableWidgetRobotType->rowCount();
    if(0 == rowCount)
    {
        return;
    }

    int row = ui->tableWidgetRobotType->currentRow();
    if(row != -1)
    {
        if(QMessageBox::Yes == QMessageBox::warning(this,"Warning","Are you sure to Delete this product type?",
                                                    QMessageBox::Yes|QMessageBox::No))
        {
            /**************删除本地配置文件的存储内容***************/
            ui->tableWidgetRobotType->removeRow(row);
        }
    }
}

void RobotDialog::on_pushButtonSaveType_clicked()
{
    if(QMessageBox::Yes == QMessageBox::warning(this,"Warning","Are you sure to Save product type?",
                                                QMessageBox::Yes|QMessageBox::No))
    {
        /**************保存配置到本地配置文件***************/


//        QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
//        configWrite->setValue(SERVER_IP_ADDRESS, ipAddress);
//        configWrite->setValue(SERVER_PORT, port);
//        configWrite->setValue(ROBOT_IP, robotIP);
//        configWrite->setValue(ROBOT_PORT, robotPort);
//        delete configWrite;
    }
}
