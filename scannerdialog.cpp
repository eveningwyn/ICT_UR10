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
    if(QMessageBox::Yes == QMessageBox::warning(this,tr("保存设置"),tr("请确认是否保存配置文件？"),
                                                QMessageBox::Yes|QMessageBox::No))
    {
        QString portName = ui->comboBoxPortNum->currentText();
        QString baudRate = ui->comboBoxBaudBits->currentText();
        QString dataBits = ui->comboBoxDataBits->currentText();
        QString parityBits = ui->comboBoxParityBits->currentText();
        QString stopBits = ui->comboBoxStopBits->currentText();

        QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        configWrite->setValue(SCANNER_PORT_NAME, portName);
        configWrite->setValue(SCANNER_BAUD_RATE, baudRate);
        configWrite->setValue(SCANNER_DATA_BITS, dataBits);
        configWrite->setValue(SCANNER_PARITY_BITS, parityBits);
        configWrite->setValue(SCANNER_STOP_BITS, stopBits);
        delete configWrite;
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

