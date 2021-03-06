﻿#include "scanneronthread.h"
#include "staticname.h"
#include <QSettings>
#include <QDateTime>
#include "language.h"
//#include <QThread>

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

   if("<T>\r\n"==sn)
   {//调试时，只需将串口2、3PIN短接即可屏蔽连接Scanner
       sn = "AH13012400\r\n";//用于调试----------------------------------------
   }

   emit forShow_To_Comm(forShowReceiveString(sn));

   if("noread\r\n"==sn.toLower())
   {
       return;
   }
   if(true == auto_Scan)
   {
       emit readSnDone(true);
       emit scanResult(sn);
   }
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
        scantimer->start(2500);
        emit forShow_To_Comm(forShowSendString(prefix+"<T>"+suffix));
        canScan = false;
    }
}

void ScannerOnThread::timerTimeOut()
{
    if(scantimer->isActive())
        scantimer->stop();

    canScan = true;
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString scan_times   = configRead->value(SCANNER_SCAN_TIMES).toString();
    delete configRead;
    if(0>=scan_times.toInt()||""==scan_times)
    {
        scan_times = "3";
    }
    if(scan_times.toInt() > scanCount)//3次扫描机会
    {
        scannerScanSN(auto_Scan);
        return;
    }

    //三次扫描失败
    scanCount = 0;
    emit scanner_Error_Msg(tr("扫描条码超时!\n"));
    if(true == auto_Scan)
    {
        emit readSnDone(false);
        emit scanError();
    }
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
    out1Timer = new QTimer(this);
    out2Timer = new QTimer(this);
    scanCount = 0;
    control_out2_count = 0;
    control_out1_count = 0;
    control_out1_timer = new QTimer(this);
    prefix = "";
    suffix = "\r\n";
    connect(scanner,&SerialPortObj::serialReadReady,this,&ScannerOnThread::scannerReadSN);
    connect(controlBoard,&SerialPortObj::serialReadReady,this,&ScannerOnThread::controlBoardRead);
    connect(scantimer,&QTimer::timeout,this,&ScannerOnThread::timerTimeOut);
    connect(checkSensorTimer,&QTimer::timeout,this,&ScannerOnThread::checkSensor);
    connect(out1Timer,&QTimer::timeout,this,&ScannerOnThread::out1TimerTimeOut);
    connect(out2Timer,&QTimer::timeout,this,&ScannerOnThread::out2TimerTimeOut);
    connect(control_out1_timer,&QTimer::timeout,this,&ScannerOnThread::control_out1_timer_timeout);

    canScan = true;
    canRead = false;
    auto_Scan = false;
    sensor1 = false;
    sensor2 = false;
    cylinderUp = false;
    not_DUT_board = false;

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
        emit scanner_Error_Msg(tr("IO控制板连接失败，请检查后重启软件！\n"));
    }
    else
    {
        emit forShow_To_Comm(forShowString(QString(tr("IO控制板:%1 已连接\n")).arg(portName_Control)));
        controlBoardWrite(CONTROL_OUT1_OFF);
        //checkSensor();//连接控制板之后，查询一次流水线Sensor状态
    }

    delete configRead;
}

void ScannerOnThread::controlBoardRead()
{//靠近阻挡气缸的传感器为1，远离阻挡气缸的传感器为2
    bool sensor1Temp = sensor1;
    bool sensor2Temp = sensor2;
    QString readStr;
    controlBoard->serialPortRead(readStr,"@","!");
    if(readStr.isEmpty())
        return;
    if("@0!"==readStr)
    {
        //流水线已有载板
        if(true==sensor2Temp && false==sensor1Temp && true==not_DUT_board)
        {
            sensor1 = true;
            sensor2 = true;
            not_DUT_board = false;
        }
    }
    else
    {
        if("@1!"==readStr)
        {
            //流水线正在上载板
            if(false==sensor2Temp && false==sensor1Temp && true==not_DUT_board)
            {
                sensor1 = false;
                sensor2 = true;
            }
        }
        else
        {
            if("@2!"==readStr)
            {
                //流水线正在出载板
                sensor1 = true;
                sensor2 = false;
                //not_DUT_board = false;
            }
            else
            {
                if("@3!"==readStr)
                {
                    //流水线无载板，可操作气缸
                    if(false==sensor2Temp && true==sensor1Temp)
                    {
                        sensor1 = false;
                        sensor2 = false;
                        not_DUT_board = true;
                    }
                }
            }
        }
    }

    //流水线状态改变时发送信号
    if(sensor1Temp != sensor1 || sensor2Temp != sensor2)
    {
        emit lineSensorStatus(sensor1,sensor2);
        if(false==sensor1)
        {
            out1Timer->start(885);
        }
    }
}

void ScannerOnThread::controlBoardWrite(QString writeMsg)
{
    controlBoard_mutex.lock();

    //writeMsg == ck,查询传感器状态 //writeMsg == on,气缸上升 //writeMsg == of,气缸下降
    controlBoard->serialPortWrite("#"+writeMsg+"*");
    if(CONTROL_CHECK==writeMsg)
    {
        controlBoard_mutex.unlock();
        return;
    }
    emit forShow_To_Comm(forShowString(QString(tr("Send_to_ControlBoard:#%1*\n")).arg(writeMsg)));

    if(CONTROL_OUT1_ON==writeMsg)
    {
        cylinderUp = true;
        controlBoard_mutex.unlock();
        return;
    }
    if(CONTROL_OUT1_OFF==writeMsg)
    {
        control_out1_count++;
        if(2>=control_out1_count)
        {
            if(!control_out1_timer->isActive())
            {
                control_out1_timer->start(243);
            }
        }
        cylinderUp = false;
        controlBoard_mutex.unlock();
        return;
    }
    if(CONTROL_OUT2_ON==writeMsg)
    {
        if(!out2Timer->isActive())
            out2Timer->start(100);
        control_out2_count++;
        controlBoard_mutex.unlock();
        return;
    }
    if(CONTROL_OUT2_OFF==writeMsg)
    {
        control_out2_count++;
        if(2!=control_out2_count && 3!=control_out2_count)
        {
            if(out2Timer->isActive())
                out2Timer->stop();
            control_out2_count = 0;
        }
        controlBoard_mutex.unlock();
        return;
    }
    controlBoard_mutex.unlock();
}

void ScannerOnThread::out1TimerTimeOut()
{
    if((false==sensor1)||(false==cylinderUp && false==sensor1))
    {
        controlBoardWrite(CONTROL_OUT1_ON);
    }
    if(out1Timer->isActive())
        out1Timer->stop();
}

void ScannerOnThread::out2TimerTimeOut()
{
    if(1==control_out2_count)
    {
        controlBoardWrite(CONTROL_OUT2_ON);
        return;
    }
    if(2!=control_out2_count)
    {
        if(out2Timer->isActive())
            out2Timer->stop();
    }
    controlBoardWrite(CONTROL_OUT2_OFF);
}

void ScannerOnThread::checkSensor()
{
    if(checkSensorTimer->isActive())
    {
        controlBoardWrite(CONTROL_CHECK);
    }
    else
    {
        checkSensorTimer->start(557);
    }
}

void ScannerOnThread::robot_Connected(bool conn)
{
    controlBoard->clearBuffer();
    controlBoardWrite(CONTROL_OUT1_OFF);
    control_out2_count = 0;
    control_out1_count = 0;
    not_DUT_board = false;
    if(true == conn)
    {
        checkSensor();
    }
    else
    {
        checkSensorTimer->stop();
    }
}

void ScannerOnThread::control_out1_timer_timeout()
{
    if(2<=control_out1_count)
    {
        if(control_out1_timer->isActive())
        {
            control_out1_timer->stop();
        }
        control_out1_count = 0;
        return;
    }
    controlBoardWrite(CONTROL_OUT1_OFF);
}
