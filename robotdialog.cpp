#include "robotdialog.h"
#include "ui_robotdialog.h"
#include <QMessageBox>
#include <QSettings>

RobotDialog::RobotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RobotDialog)
{
    ui->setupUi(this);
    initialize();
}

RobotDialog::~RobotDialog()
{
    delete ui;
}

void RobotDialog::initialize()
{
    QSettings *configRead = new QSettings("..\\path/Config.ini", QSettings::IniFormat);
    QString ipAddress   = configRead->value("/RobotParameter/IPaddress").toString();
    QString port = configRead->value("/RobotParameter/Port").toString();
    QString robotIP = configRead->value("/RobotParameter/RobotIP").toString();
    QString robotPort = configRead->value("/RobotParameter/RobotPort").toString();
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

        QSettings *configWrite = new QSettings("..\\path/Config.ini", QSettings::IniFormat);
        configWrite->setValue("/RobotParameter/IPaddress", ipAddress);
        configWrite->setValue("RobotParameter/Port", port);
        configWrite->setValue("RobotParameter/RobotIP", robotIP);
        configWrite->setValue("RobotParameter/RobotPort", robotPort);
        delete configWrite;
    }
}
