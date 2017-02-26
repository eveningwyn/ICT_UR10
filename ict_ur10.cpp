#include "ict_ur10.h"
#include "ui_ict_ur10.h"
#include "scannerdialog.h"
#include "robotdialog.h"
#include <QSettings>
#include <QMessageBox>

ICT_UR10::ICT_UR10(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ICT_UR10)
{
    ui->setupUi(this);

    scan_thread = new ScannerThread(this);//实例scan线程对象
    robot_thread = new RobotThread(this);//实例robot线程对象

    scanner = new SerialPortWidget(this);
    initialize();

    commDlg = new CommunicationDialog(this);
    isShow = false;

    connect(robot_thread,SIGNAL(startScan()),scan_thread,SLOT(scannerScanSN()));
    connect(this,SIGNAL(manualScan()),scan_thread,SLOT(scannerScanSN()));
    connect(scanner,SIGNAL(serialReadReady()),scan_thread,SLOT(scannerReadSN()));
    connect(scan_thread,SIGNAL(scanResult(QString)),this,SLOT(getSn(QString)));
    connect(scan_thread,SIGNAL(scanError(QString)),this,SLOT(errorMessage(QString)));

    scan_thread->start();
    robot_thread->start();
}

ICT_UR10::~ICT_UR10()
{
    if(scan_thread->isRunning())
        scan_thread->stop();
    if(robot_thread->isRunning())
        robot_thread->stop();
    QThread::msleep(100);
    delete scan_thread;
    delete robot_thread;
    delete ui;
}

void ICT_UR10::on_actionScanner_triggered()
{
    ScannerDialog scanDlg(this);
    emit sig();
    scanDlg.exec();
}

void ICT_UR10::on_actionRobot_triggered()
{
    RobotDialog robotDlg(this);
    emit sig();
    robotDlg.exec();
}

void ICT_UR10::on_actionCommunication_triggered()
{
    if(false == isShow)
    {
        commDlg->show();
        isShow = true;
    }
    else
    {
        commDlg->hide();
        isShow = false;
    }
}

void ICT_UR10::initialize()
{
    QSettings *configRead = new QSettings("..\\path/Config.ini", QSettings::IniFormat);
    QString portName = configRead->value("/ScannerParameter/PortName").toString();
    int baudRate = configRead->value("/ScannerParameter/BaudRate").toString().toInt();
    int dataBits = configRead->value("/ScannerParameter/DataBits").toString().toInt();
    QString parityBits = configRead->value("/ScannerParameter/ParityBits").toString();
    QString stopBits = configRead->value("/ScannerParameter/StopBits").toString();
    delete configRead;
    if(!(scanner->openSerialPort(portName,baudRate,dataBits,parityBits,stopBits,true,true)))
    {
        QMessageBox::warning(this,tr("Error"),tr("Scanner initialize failed!"),QMessageBox::Ok);
        ui->labelScanStatus->setText("Scanner Disconnected!");
    }
    else
        ui->labelScanStatus->setText("Scanner Connected");
}

void ICT_UR10::getSn(QString sn)
{
    if(ui->lineEditSN->text().isEmpty())
    {
        sn.replace("\r","");
        sn.replace("\n","");
        ui->lineEditSN->setText(sn);
    }
}

void ICT_UR10::manualStartScan()
{
    emit manualScan();
}

void ICT_UR10::errorMessage(QString errorMsg)
{
    QMessageBox::warning(this,tr("Error"),QString("%1").arg(errorMsg),QMessageBox::Ok);
}
