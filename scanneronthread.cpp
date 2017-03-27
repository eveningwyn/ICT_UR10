#include "scanneronthread.h"
#include "staticname.h"
#include <QSettings>
#include <QDateTime>
#include "language.h"
#include <QThread>

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

   sn = "AH12016602\r\n";//用于调试----------------------------------------

   if("noread\r\n"==sn.toLower())
   {
       return;
   }

   if(true == auto_Scan)
       emit scanResult(sn);
   emit forShow_To_Comm(forShowReceiveString(sn));
   scantimer->stop();

   scanner->clearBuffer();
   canScan = true;
   canRead = false;
}

void ScannerOnThread::scannerScanSN(bool autoScan)
{
    if(!scanner->serialPortIsOpen())
    {
        emit scanner_Error_Msg(tr("扫描枪未连接，请检查后重启软件！\n"));
        return;
    }
    if(true == canScan)
    {
        auto_Scan = autoScan;
        canRead = true;
        scanner->clearBuffer();
        scanner->serialPortWrite(prefix+"<T>"+suffix);
        scanCount++;
        if(scantimer->isActive())
            scantimer->stop();
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
        scannerScanSN(auto_Scan);
        return;
    }

    //三次扫描失败
    scanCount = 0;
    emit scanner_Error_Msg(tr("扫描条码超时!\n"));
    if(true == auto_Scan)
        emit scanError();
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
    controlBoard = new SerialPortObj(this);
    scantimer = new QTimer(this);
    checkSensorTimer = new QTimer(this);
    out2Timer = new QTimer(this);
    scanCount = 0;
    prefix = "";
    suffix = "\r\n";
    connect(scanner,&SerialPortObj::serialReadReady,this,&ScannerOnThread::scannerReadSN);
    connect(controlBoard,&SerialPortObj::serialReadReady,this,&ScannerOnThread::controlBoardRead);
    connect(scantimer,&QTimer::timeout,this,&ScannerOnThread::timerTimeOut);
    connect(checkSensorTimer,&QTimer::timeout,this,&ScannerOnThread::checkSensor);
    connect(out2Timer,&QTimer::timeout,this,&ScannerOnThread::out2TimerTimeOut);

    canScan = true;
    canRead = false;
    auto_Scan = false;
    checkSensorTimer->start(500);
    lineIsReady = false;
    lineIsNoBoard = true;

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
        emit scannerIsReady(false);
        emit scanner_Error_Msg(tr("扫描枪连接失败，请检查后重启软件！\n"));
    }
    else
    {
        emit scanner_Status(QString(tr("扫描器:%1 已连接")).arg(portName));
        emit scannerIsReady(true);
        emit forShow_To_Comm(forShowString(QString(tr("扫描器:%1 已连接\n")).arg(portName)));
    }


    QString portName_Control = configRead->value(SCANNER_PORT_NAME_CONTROL).toString();
    int baudRate_Control = configRead->value(SCANNER_BAUD_RATE_CONTROL).toString().toInt();
    int dataBits_Control = configRead->value(SCANNER_DATA_BITS_CONTROL).toString().toInt();
    QString parityBits_Control = configRead->value(SCANNER_PARITY_BITS_CONTROL).toString();
    QString stopBits_Control = configRead->value(SCANNER_STOP_BITS_CONTROL).toString();
    if(!(controlBoard->openSerialPort(portName_Control,baudRate_Control,dataBits_Control,parityBits_Control,stopBits_Control,true,true)))
    {
        emit scanner_Error_Msg(tr("阻挡气缸控制板连接失败，请检查后重启软件！\n"));
    }
    else
    {
        emit forShow_To_Comm(forShowString(QString(tr("阻挡气缸控制板:%1 已连接\n")).arg(portName_Control)));
        checkSensor();//连接控制板之后，查询一次流水线Sensor状态
    }

    delete configRead;
}

void ScannerOnThread::controlBoardRead()
{
    bool lineIsReadyTemp = lineIsReady;
    bool lineIsNoBoardTemp = lineIsNoBoard;
    QString readStr;
    controlBoard->serialPortRead(readStr,"@","!");
    if(readStr.isEmpty())
        return;
    if("@0!"==readStr)
    {
        //流水线有载板，气缸已上升
        lineIsReady = true;
    }
    else
    {
        if("@1!"==readStr)
        {
            //流水线无载板，气缸已上升
            lineIsReady = false;
        }
        else
        {
            if("@2!"==readStr)
            {
                //流水线有载板，气缸已下降
                lineIsNoBoard = false;
            }
            else
            {
                if("@3!"==readStr)
                {
                    //流水线无载板，气缸已下降
                    lineIsNoBoard = true;
                }
            }
        }
    }
    //流水线状态改变时发送信号
    if(lineIsReadyTemp != lineIsReady)
    {
        emit lineReady(lineIsReady);
    }
    if(lineIsNoBoardTemp != lineIsNoBoard)
    {
        emit lineNoBoard(lineIsNoBoard);
    }
}

void ScannerOnThread::controlBoardWrite(QString writeMsg)
{//writeMsg == ck,查询传感器状态//writeMsg == on,气缸上升//writeMsg == of,气缸下降
    controlBoard->serialPortWrite("#"+writeMsg+"*");
    if(CONTROL_OUT2_ON==writeMsg)
    {
        if(!out2Timer->isActive())
        {
            out2Timer->start(100);
        }
    }
}

void ScannerOnThread::out2TimerTimeOut()
{
    if(out2Timer->isActive())
    {
        out2Timer->stop();
    }
    controlBoardWrite(CONTROL_OUT2_OFF);
}

void ScannerOnThread::checkSensor()
{
    controlBoardWrite(CONTROL_CHECK);
}
