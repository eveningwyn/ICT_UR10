#include "scannerdialog.h"
#include "ui_scannerdialog.h"
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSettings>
#include "staticname.h"
#include "language.h"

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
    if(QMessageBox::Yes == QMessageBox::warning(this,tr("保存配置"),tr("请确认是否保存配置文件？"),
                                                QMessageBox::Yes|QMessageBox::No))
    {
        saveConfig();
        QMessageBox::warning(this,tr("保存配置"),tr("保存配置成功！\n"),QMessageBox::Ok);
    }
}

void ScannerDialog::initializeInfo()
{
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString portName   = configRead->value(SCANNER_PORT_NAME).toString();
    QString baudRate   = configRead->value(SCANNER_BAUD_RATE).toString();
    QString dataBits   = configRead->value(SCANNER_DATA_BITS).toString();
    QString parityBits = configRead->value(SCANNER_PARITY_BITS).toString();
    QString stopBits   = configRead->value(SCANNER_STOP_BITS).toString();

    QString portName_control   = configRead->value(SCANNER_PORT_NAME_CONTROL).toString();
    QString baudRate_control   = configRead->value(SCANNER_BAUD_RATE_CONTROL).toString();
    QString dataBits_control   = configRead->value(SCANNER_DATA_BITS_CONTROL).toString();
    QString parityBits_control = configRead->value(SCANNER_PARITY_BITS_CONTROL).toString();
    QString stopBits_control   = configRead->value(SCANNER_STOP_BITS_CONTROL).toString();
    delete configRead;
    int flag = 0;
    if(""==portName)
    {
        portName = "COM1";
        flag++;
    }
    if(""==baudRate)
    {
        baudRate = "115200";
        flag++;
    }
    if(""==dataBits)
    {
        dataBits = "8";
        flag++;
    }
    if(""==parityBits)
    {
        parityBits = "none";
        flag++;
    }
    if(""==stopBits)
    {
        stopBits = "1";
        flag++;
    }

    if(""==portName_control)
    {
        portName_control = "COM2";
        flag++;
    }
    if(""==baudRate_control)
    {
        baudRate_control = "19200";
        flag++;
    }
    if(""==dataBits_control)
    {
        dataBits_control = "8";
        flag++;
    }
    if(""==parityBits_control)
    {
        parityBits_control = "none";
        flag++;
    }
    if(""==stopBits_control)
    {
        stopBits_control = "1";
        flag++;
    }

    ui->comboBoxPortNum->addItem(portName);
    ui->comboBoxBaudBits->setItemText(0,baudRate);
    ui->comboBoxDataBits->setItemText(0,dataBits);
    ui->comboBoxParityBits->setItemText(0,parityBits);
    ui->comboBoxStopBits->setItemText(0,stopBits);
    ui->comboBoxPortNum_Control->addItem(portName_control);
    ui->comboBoxBaudBits_Control->setItemText(0,baudRate_control);
    ui->comboBoxDataBits_Control->setItemText(0,dataBits_control);
    ui->comboBoxParityBits_Control->setItemText(0,parityBits_control);
    ui->comboBoxStopBits_Control->setItemText(0,stopBits_control);

    if(0<flag)
    {
        saveConfig();
        QMessageBox::warning(this,tr("提示"),tr("串口配置有部分参数已恢复默认设置，请检查参数配置！\n"),QMessageBox::Ok);
    }
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
        ui->comboBoxPortNum_Control->clear();
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

        for(i=0;i<ui->comboBoxPortNum_Control->count();i++)
        {
            if(info.portName()==ui->comboBoxPortNum_Control->itemText(i))
                break;
        }
        if(ui->comboBoxPortNum_Control->count()==i)
        {
            ui->comboBoxPortNum_Control->addItem(info.portName());
        }
    }
}

void ScannerDialog::saveConfig()
{
    QString portName = ui->comboBoxPortNum->currentText();
    QString baudRate = ui->comboBoxBaudBits->currentText();
    QString dataBits = ui->comboBoxDataBits->currentText();
    QString parityBits = ui->comboBoxParityBits->currentText();
    QString stopBits = ui->comboBoxStopBits->currentText();
    QString portName_control = ui->comboBoxPortNum_Control->currentText();
    QString baudRate_control = ui->comboBoxBaudBits_Control->currentText();
    QString dataBits_control = ui->comboBoxDataBits_Control->currentText();
    QString parityBits_control = ui->comboBoxParityBits_Control->currentText();
    QString stopBits_control = ui->comboBoxStopBits_Control->currentText();

    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    configWrite->setValue(SCANNER_PORT_NAME, portName);
    configWrite->setValue(SCANNER_BAUD_RATE, baudRate);
    configWrite->setValue(SCANNER_DATA_BITS, dataBits);
    configWrite->setValue(SCANNER_PARITY_BITS, parityBits);
    configWrite->setValue(SCANNER_STOP_BITS, stopBits);
    configWrite->setValue(SCANNER_PORT_NAME_CONTROL, portName_control);
    configWrite->setValue(SCANNER_BAUD_RATE_CONTROL, baudRate_control);
    configWrite->setValue(SCANNER_DATA_BITS_CONTROL, dataBits_control);
    configWrite->setValue(SCANNER_PARITY_BITS_CONTROL, parityBits_control);
    configWrite->setValue(SCANNER_STOP_BITS_CONTROL, stopBits_control);
    delete configWrite;
}

void ScannerDialog::on_pushButtonRefresh_Control_clicked()
{
    getPortName();
}
