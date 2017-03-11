#include "scanneronthread.h"
#include "staticname.h"
#include <QSettings>
#include <QDateTime>
#include "language.h"

#include <QDebug>

ScannerOnThread::ScannerOnThread(QObject *parent) : QObject(parent)
{
}

void ScannerOnThread::scannerReadSN()
{
    if(false == canRead)
    {
        scanner->clearBuffer();
        return;
    }
   QString sn;
   scanner->serialPortRead(sn,prefix,suffix);
   if(sn.isEmpty())
       return;
   if("noread\r\n"==sn.toLower())
   {
//       qDebug()<<"scan:"<<sn;
   }

   sn = "SN1234567890\r\n";

   emit scanResult(sn);
   emit forShow_To_Comm(forShowReceiveString(sn));
   scantimer->stop();

   scanner->clearBuffer();
   canScan = true;
   canRead = false;
}

void ScannerOnThread::scannerScanSN()
{
    if(!scanner->serialPortIsOpen())
    {
        emit scanner_Error_Msg(tr("扫描枪未连接，请检查后重启软件！\n"));
        return;
    }
    if(true == canScan)
    {
        canRead = true;
        scanner->clearBuffer();
        scanner->serialPortWrite(prefix+"<T>"+suffix);
        scanCount++;
        scantimer->start(2200);
        emit forShow_To_Comm(forShowSendString(prefix+"<T>"+suffix));
        canScan = false;
    }
}

void ScannerOnThread::timerTimeOut()
{
    if(scantimer->isActive())
        scantimer->stop();

    canScan = true;
    if(3 > scanCount)//3次扫描机会
    {
        scannerScanSN();
        return;
    }

    //三次扫描失败
    scanCount = 0;
    emit scanner_Error_Msg(tr("扫描条码超时!\n"));
}

QString ScannerOnThread::forShowReceiveString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Receive_from_Scanner:" + str;
    return str;
}

QString ScannerOnThread::forShowSendString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Send_to_Scanner:" + str;
    return str;
}

QString ScannerOnThread::forShowString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + str + "\n";
    return str;
}

void ScannerOnThread::init_Scanner()
{
    scanner = new SerialPortObj(this);
    scantimer = new QTimer(this);
    scanCount = 0;
    prefix = "";
    suffix = "\r\n";
    connect(scanner,&SerialPortObj::serialReadReady,this,&ScannerOnThread::scannerReadSN);
    connect(scantimer,&QTimer::timeout,this,&ScannerOnThread::timerTimeOut);

    canScan = true;
    canRead = false;

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    //Scanner
    QString portName = configRead->value(SCANNER_PORT_NAME).toString();
    int baudRate = configRead->value(SCANNER_BAUD_RATE).toString().toInt();
    int dataBits = configRead->value(SCANNER_DATA_BITS).toString().toInt();
    QString parityBits = configRead->value(SCANNER_PARITY_BITS).toString();
    QString stopBits = configRead->value(SCANNER_STOP_BITS).toString();
    if(!(scanner->openSerialPort(portName,baudRate,dataBits,parityBits,stopBits,true,true)))
    {
        emit scanner_Status(QString(tr("扫描器:%1 已断开")).arg(portName));
        emit scanner_Error_Msg(tr("扫描枪连接失败，请检查后重启软件！\n"));
    }
    else
    {
        emit scanner_Status(QString(tr("扫描器:%1 已连接")).arg(portName));
        emit forShow_To_Comm(forShowString(QString(tr("扫描器:%1 已连接\n")).arg(portName)));
    }

    delete configRead;
}
