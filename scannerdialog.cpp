﻿#include "scannerdialog.h"
#include "ui_scannerdialog.h"
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include "ict_ur10.h"

ScannerDialog::ScannerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScannerDialog)
{
    ui->setupUi(this);
    initializeInfo();
}

ScannerDialog::~ScannerDialog()
{
    delete ui;
}
void ScannerDialog::on_pushButtonSetting_clicked()
{
    if(QMessageBox::Yes == QMessageBox::warning(this,"Setting","Are you sure to configuration parameters?",
                                                QMessageBox::Yes|QMessageBox::No))
    {
        QString portName = ui->comboBoxPortNum->currentText();
        QString baudRate = ui->comboBoxBaudBits->currentText();
        QString dataBits = ui->comboBoxDataBits->currentText();
        QString parityBits = ui->comboBoxParityBits->currentText();
        QString stopBits = ui->comboBoxStopBits->currentText();

        QSettings *configWrite = new QSettings("..\\path/Config.ini", QSettings::IniFormat);
        configWrite->setValue("/ScannerParameter/PortName", portName);
        configWrite->setValue("ScannerParameter/BaudRate", baudRate);
        configWrite->setValue("ScannerParameter/DataBits", dataBits);
        configWrite->setValue("ScannerParameter/ParityBits", parityBits);
        configWrite->setValue("ScannerParameter/StopBits", stopBits);
        delete configWrite;
    }
}

void ScannerDialog::initializeInfo()
{
    QSettings *configRead = new QSettings("..\\path/Config.ini", QSettings::IniFormat);
    QString portName   = configRead->value("/ScannerParameter/PortName").toString();
    QString baudRate   = configRead->value("/ScannerParameter/BaudRate").toString();
    QString dataBits   = configRead->value("/ScannerParameter/DataBits").toString();
    QString parityBits = configRead->value("/ScannerParameter/ParityBits").toString();
    QString stopBits   = configRead->value("/ScannerParameter/StopBits").toString();
    delete configRead;

    ui->comboBoxPortNum->addItem(portName);
    ui->comboBoxBaudBits->addItem(baudRate);
    ui->comboBoxDataBits->addItem(dataBits);
    ui->comboBoxParityBits->addItem(parityBits);
    ui->comboBoxStopBits->addItem(stopBits);
}

void ScannerDialog::on_pushButtonRefresh_clicked()
{
    getPortName();
}

void ScannerDialog::getPortName()
{
    QList<QSerialPortInfo> portInfoList = QSerialPortInfo::availablePorts();
    if(portInfoList.isEmpty())
    {
        ui->comboBoxPortNum->clear();
        return;
    }
    foreach (QSerialPortInfo info, portInfoList)
    {
        int i=0;
        for(;i<ui->comboBoxPortNum->count();i++)
        {
            if(info.portName()==ui->comboBoxPortNum->itemText(i))
                break;
        }
        if(ui->comboBoxPortNum->count()==i)
        {
            ui->comboBoxPortNum->addItem(info.portName());
        }
    }
}

void ScannerDialog::on_pushButtonOpen_clicked()
{
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    QString portName = ui->comboBoxPortNum->currentText();
    int baudRate = ui->comboBoxBaudBits->currentText().toInt();
    int dataBits = ui->comboBoxDataBits->currentText().toInt();
    QString parityBits = ui->comboBoxParityBits->currentText();
    QString stopBits = ui->comboBoxStopBits->currentText();
    if(!(ptr->scanner->openSerialPort(portName,baudRate,dataBits,parityBits,stopBits,true,true)))
    {
        QMessageBox::warning(this,tr("Error"),tr("Scanner open failed!"),QMessageBox::Ok);
        return;
    }
    ptr->updateScannerStatue(portName,true);
}
