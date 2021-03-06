﻿#include "robotonthread.h"
#include "staticname.h"
#include <QDateTime>
#include <QSettings>
#include "language.h"
//#include <QThread>

#define TIMEOUT_SEC  1700

RobotOnThread::RobotOnThread(QObject *parent) : QObject(parent)
{
}

QString RobotOnThread::forShowReceiveString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Receive_from_Robot:" + str + "\n";
    return str;
}

QString RobotOnThread::forShowSendString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Send_to_Robot:" + str + "\n";
    return str;
}

QString RobotOnThread::forShowString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + str + "\n";
    return str;
}

void RobotOnThread::robotReadData(QString IP, int Port, QString readMsg)
{
    robot_read_mutex.lock();
    emit forShow_To_Comm(forShowReceiveString(QString("%1 %2:%3").arg(IP).arg(Port).arg(readMsg)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP ==IP && robotPort==QString("%1").arg(Port))
    {
        informationCheck(readMsg);//根据协议处理接收的数据
    }
    robot_read_mutex.unlock();
}

void RobotOnThread::robotSendMsg(QString sendMsg)
{
    robot_send_mutex.lock();

    if(true == robotPortExist)
    {
        sendMsg.replace(SUFFIX,"");
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString robotPort = configRead->value(ROBOT_PORT).toString();
        delete configRead;

        robotServer->sendData(robotPort.toInt(),sendMsg);

        emit forShow_To_Comm(forShowSendString(sendMsg));
    }
    else
    {
        serverSendError();
        emit forShow_To_Comm(forShowSendString(tr("发送中断，Robot未连接！")));
    }

    robot_send_mutex.unlock();
}

void RobotOnThread::informationCheck(QString msg)//根据协议处理接收的数据
{
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Robot init ACK")))
    {
        if(initTimer->isActive())
            initTimer->stop();
        emit robot_Status(tr("机器人:初始化运行中..."));
        return;
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Robot init done")))
    {
        robotInitDone = true;
        emit robot_Status(tr("机器人:初始化完成"));
        emit robotReady(true);
        emit setRunStatus(false);
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot init done ACK"));
        return;
    }
    if(true == robotAutoMode)//自动模式
    {
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("PC status?")))
        {
            serverSendError();
            if(true == PC_Is_Ready)
            {
                emit setRunStatus(true);
                robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("PC ready"));
                emit robot_catchFail();
                return;
            }
            else
            {
                if(false == PC_Is_Ready)
                {
                    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("PC not ready"));
                    return;
                }
            }
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Scan ready")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan ready ACK"));
            emit startScan(true);
            return;
        }
        if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Read SN Done ACK")))
                ||(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("No Read ACK"))))
        {
            scanDoneState = true;
            snCheckDoneState = false;
            if(snReadTimer->isActive())
                snReadTimer->stop();
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Scan done ACK")))
        {
            scanDoneState = false;
            snCheckDoneState = true;
            if(snResultTimer->isActive())
                snResultTimer->stop();
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Scan error ACK")))
        {
            scanDoneState = false;
            snCheckDoneState = true;
            if(scanErrorTimer->isActive())
                scanErrorTimer->stop();
            emit setRunStatus(false);
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Test ready")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Test ready ACK"));
            emit startTest();//发出开始测试信号
            return;
        }
        if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Pass done ACK"))) ||
                (0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Fail done ACK"))))
        {
            if(testResultTimer->isActive())
                testResultTimer->stop();
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Sort complete")))
        {
            if(true==testPass)
            {
//                emit cylinderUpDown(CONTROL_OUT1_OFF);
                QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
                QString line_wait_time_Str = configRead->value(SCANNER_LINE_WAIT_TIME).toString();
                delete configRead;
                bool ok;
                int line_wait_time = line_wait_time_Str.toInt(&ok,10);
                if(!ok)
                    line_wait_time = 1;
                if(!cylinderDownTimer->isActive())
                    cylinderDownTimer->start(line_wait_time);

                lineSensor1 = false;
                lineSensor2 = false;
            }
            emit setRunStatus(false);
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Sort complete ACK"));
            emit sortComplete(testPass);//发出分拣完成信号
            checkPass = false;
            barcode = "";
            testPass = false;
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Line state?")))
        {
//            robotGetLineState = true;
//            infromLineInfoToRobot();
            return;
        }
    }
    else//手动模式
    {
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Move to Scan done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Move to Scan done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Fixture pickup done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fixture pickup done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Fixture place done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fixture place done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT place done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT place done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT pickup done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT pickup done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT close done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT close done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT open done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT open done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Place OK done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Place OK done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Place NG done")))
        {
            emit debugRunDone();
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Place NG done ACK"));
            return;
        }
    }
    //手动/自动模式共用通讯协议部分
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Error")))
    {
        msg.replace(SUFFIX,"");
        robotSendMsg(QString("%1 ACK%2").arg(msg).arg(SUFFIX));
        emit robot_Error_Msg(msg+SUFFIX+"\n");//发出报错信息
        return;
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Robot return ACK")))
    {
        emit setRunStatus(false);
        if(returnResultTimer->isActive())
            returnResultTimer->stop();
        return;
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Robot return done")))
    {
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot return done ACK"));
        return;
    }
    if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Auto mode ACK"))) ||
            (0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Debug mode ACK"))))
    {
        if(setRunModeTimer->isActive())
            setRunModeTimer->stop();
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Auto mode ACK")))
        {
            emit change_auto_debug_label(tr("自动状态..."));
        }
        else
        {
            if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Debug mode ACK")))
            {
                emit change_auto_debug_label(tr("调试状态..."));
            }
        }
        return;
    }
    if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Line ready ACK"))) ||
            (0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Line not ready ACK"))))
    {
        if(infoLineReadyTimer->isActive())
            infoLineReadyTimer->stop();
        return;
    }
}

void RobotOnThread::init_Robot()
{
    initTimer = new QTimer(this);
    snReadTimer = new QTimer(this);
    snResultTimer = new QTimer(this);
    scanErrorTimer = new QTimer(this);
    testResultTimer = new QTimer(this);
    returnResultTimer = new QTimer(this);
    setProtTimer = new QTimer(this);
    setRunModeTimer = new QTimer(this);
    infoLineReadyTimer = new QTimer(this);
    cylinderDownTimer = new QTimer(this);

    PC_Is_Ready = false;
    readedSN = false;
    checkPass = false;
    barcode = "";
    testPass = false;
    robot_pro_num = "";
//    robotAutoMode = true;//初始化为自动模式
    robotAutoMode = false;//初始化为手动模式
    robotPortExist = false;

    lineSensor1 = false;
    lineSensor2 = false;
//    robotGetLineState = false;

    robotServer = new TcpIpServer(this);
    robotServer->set_prefix_suffix(PREFIX,SUFFIX);

    robotClient = new TcpIpClient(this);
    robotClient->prefix = "";
    robotClient->suffix = "\n";
    robot_start_timer = new QTimer(this);
    robot_pause_timer = new QTimer(this);
    robot_stop_timer = new QTimer(this);
    dashboard_enable = false;

    connect(robotClient,&TcpIpClient::readData,this,&RobotOnThread::robot_readData);
    connect(robotClient,&TcpIpClient::cliendErrorMsg,this,&RobotOnThread::robot_Error_Msg);
    connect(robotClient,&TcpIpClient::clientDisConnect,this,&RobotOnThread::robot_clientDisConnect);

    connect(robotServer,&TcpIpServer::errorMessage,this,&RobotOnThread::robot_Error_Msg);
    connect(robotServer,&TcpIpServer::clientConnect,this,&RobotOnThread::robotConnected);
    connect(robotServer,&TcpIpServer::clientDisconnected,this,&RobotOnThread::robotDisconnected);
    connect(robotServer,&TcpIpServer::serverReadData,this,&RobotOnThread::robotReadData);

    connect(initTimer,&QTimer::timeout,this,&RobotOnThread::robot_Init);
    connect(snReadTimer,&QTimer::timeout,this,&RobotOnThread::readSnDoneTimeout);
    connect(snResultTimer,&QTimer::timeout,this,&RobotOnThread::scanDone);
    connect(scanErrorTimer,&QTimer::timeout,this,&RobotOnThread::scanError);
    connect(testResultTimer,&QTimer::timeout,this,&RobotOnThread::testDone);
    connect(returnResultTimer,&QTimer::timeout,this,&RobotOnThread::roborReturn);
    connect(setProtTimer,&QTimer::timeout,this,&RobotOnThread::setPro_Num_Timeout);
    connect(setRunModeTimer,&QTimer::timeout,this,&RobotOnThread::setRunModeTimeout);
    connect(infoLineReadyTimer,&QTimer::timeout,this,&RobotOnThread::infromLineInfoToRobot);
    connect(robot_start_timer,&QTimer::timeout,this,&RobotOnThread::robot_start);
    connect(robot_pause_timer,&QTimer::timeout,this,&RobotOnThread::robot_pause);
    connect(robot_stop_timer,&QTimer::timeout,this,&RobotOnThread::robot_stop);
    connect(cylinderDownTimer,&QTimer::timeout,this,&RobotOnThread::cylinderDownTimerTimeout);

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    //Robot
    QString ipAddress =configRead->value(SERVER_IP_ADDRESS).toString();
    quint16 port =(quint16) configRead->value(SERVER_PORT).toString().toInt();
    QString robotIP = configRead->value(ROBOT_IP).toString();
    delete configRead;

    //客户端连接Robot服务器
    if(!robotClient->newConnect(robotIP,29999))
    {
        emit robot_Status(tr("机器人:已断开"));
        emit robot_Error_Msg(tr("Dashboard Server创建失败,请检查网络连接之后重启软件!\n"));
        dashboard_enable = false;
    }
    else
    {
        dashboard_enable = true;
        emit forShow_To_Comm(forShowString(QString(tr("Dashboard Server功能已开启!"))));
    }
    //开启服务器
    if(!robotServer->stratListen(ipAddress,port))
    {
        emit robot_Status(tr("机器人:已断开"));
        emit robot_Error_Msg(tr("服务器创建失败,请检查后重启软件！\n"));
    }
    else
    {
        emit robot_Status(tr("机器人:PC服务器 Listening..."));
        emit forShow_To_Comm(forShowString(QString(tr("PC服务器 %1 %2 Listening..."))
                                           .arg(ipAddress).arg(port)));
    }
}

/*通讯协议处理部分-主动发送*/
void RobotOnThread::robot_Init()
{
    robotInitDone = false;
    if(initTimer->isActive())
        initTimer->stop();
    initTimer->start(TIMEOUT_SEC);
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot init"));
}

void RobotOnThread::readSnDone(bool readed)
{
    readedSN = readed;
    readSnDoneTimeout();
}

void RobotOnThread::readSnDoneTimeout()
{
    if(snReadTimer->isActive())
        snReadTimer->stop();
    snReadTimer->start(TIMEOUT_SEC+50);
    if(true == readedSN)
    {
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Read SN Done"));
    }
    else
    {
        snReadTimer->stop();//调试用----------------------------------
        scanDoneState = true;//调试用---------------------------------
        //robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("No Read"));
    }
}

void RobotOnThread::snCheckResult(QString sn,bool checkResult)
{
    barcode = sn;
    checkPass = checkResult;
    if(true == robotAutoMode)
    {
        scanDone();
    }
}

void RobotOnThread::scanDone()
{
    if(snResultTimer->isActive())
        snResultTimer->stop();
    if(true == checkPass)
    {
        snResultTimer->start(TIMEOUT_SEC*1.7);
        if(true==scanDoneState)
        {
            if(!snReadTimer->isActive())
                robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan done"));
        }
    }
    else
    {
//        barcode = "";
        scanError();
        return;
    }
}

void RobotOnThread::scanError()
{
    if(scanErrorTimer->isActive())
        scanErrorTimer->stop();
    scanErrorTimer->start(TIMEOUT_SEC*1.7);
    if(true==scanDoneState)
    {
        barcode = "";
        if(!snReadTimer->isActive())
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan error"));
    }
}

void RobotOnThread::testResult(QString result)
{
    if(result.contains("PASS"))
    {
        testPass = true;
    }
    else
        if(result.contains("FAIL"))
        {
            testPass = false;
        }
    emit recordTestResult(barcode,result);
    testDone();
}

void RobotOnThread::testDone()
{
    if(testResultTimer->isActive())
        testResultTimer->stop();
    testResultTimer->start(TIMEOUT_SEC+135);
    if(true==snCheckDoneState)
    {
        if(true == testPass)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Pass done"));
        }
        else
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fail done"));
        }
    }
}

void RobotOnThread::roborReturn()
{
    if(returnResultTimer->isActive())
        returnResultTimer->stop();
    returnResultTimer->start(TIMEOUT_SEC+90);
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot return"));
}

void RobotOnThread::set_PC_Status(bool isReady)
{
    PC_Is_Ready = isReady;
}

void RobotOnThread::serverSendError()
{
    initTimer->stop();
    snReadTimer->stop();
    snResultTimer->stop();
    scanErrorTimer->stop();
    testResultTimer->stop();
    returnResultTimer->stop();
    setProtTimer->stop();
    setRunModeTimer->stop();
    infoLineReadyTimer->stop();
}

void RobotOnThread::setRunModeTimeout()
{
    if(setRunModeTimer->isActive())
        setRunModeTimer->stop();
    setRobotRunMode(robotAutoMode);
}

void RobotOnThread::setRobotRunMode(bool autoMode)
{
    snReadTimer->stop();
    snResultTimer->stop();
    scanErrorTimer->stop();
    testResultTimer->stop();
    returnResultTimer->stop();

    bool modeTemp = robotAutoMode;
    robotAutoMode = autoMode;
    if(false == modeTemp && true == robotAutoMode)
    {
        robot_Init();//Debug模式切换到Auto模式后，Robot需复位
    }
    setRunModeTimer->start(TIMEOUT_SEC + 205);
    if(true == robotInitDone)
    {
        if(true == robotAutoMode)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Auto mode"));
            emit robot_Status(tr("机器人:自动运行中..."));
        }
        else
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Debug mode"));
            emit robot_Status(tr("机器人:手动运行中..."));
        }
    }
}

void RobotOnThread::setrobotPortExist(bool robot_exist)
{
    scanDoneState = false;
    snCheckDoneState = false;
    robotPortExist = robot_exist;
    if(false == robot_exist)
    {
        serverSendError();
        PC_Is_Ready = false;
        checkPass = false;
        barcode = "";
        testPass = false;
        robotAutoMode = false;//初始化为手动模式
    }
    emit robot_catchFail();//Robot断开，让ICT复位
}

void RobotOnThread::debug_moveToScan()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Move to Scan"));
}

void RobotOnThread::debug_fixturePickup()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fixture pickup"));
}

void RobotOnThread::debug_fixturePlace()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fixture place"));
}

void RobotOnThread::debug_ictPlace()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT place"));
}

void RobotOnThread::debug_ictPickup()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT pickup"));
}

void RobotOnThread::debug_ictClose()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT close"));
}

void RobotOnThread::debug_ictOpen()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT open"));
}

void RobotOnThread::debug_placeOKPos()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Place OK"));
}

void RobotOnThread::debug_placeNGPos()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Place NG"));
}

void RobotOnThread::debug_returnSafePos()
{
    roborReturn();
}

void RobotOnThread::debug_clawOpen()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Claw open"));
}

void RobotOnThread::debug_clawClose()
{
    if(true == robotAutoMode)
        return;
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Claw close"));
}

void RobotOnThread::set_light_Red_Green_Yellow_Buzzer(QString msg)
{
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg(msg));
}

void RobotOnThread::lineSensorStatus(bool sensor1True, bool sensor2True)
{//靠近阻挡气缸的传感器为1，远离阻挡气缸的传感器为2
//    bool sensorTemp1 = lineSensor1;
//    bool sensorTemp2 = lineSensor2;
//    lineSensor2 = sensor1True;
//    lineSensor1 = sensor2True;
//    if(sensorTemp1!=lineSensor1 || sensorTemp2!=lineSensor2)
//    {
//        if(infoLineReadyTimer->isActive())
//            infoLineReadyTimer->stop();
//        infromLineInfoToRobot();
//    }

    lineSensor1 = sensor1True;
    lineSensor2 = sensor2True;
    if(infoLineReadyTimer->isActive())
        infoLineReadyTimer->stop();
    infromLineInfoToRobot();
}

void RobotOnThread::infromLineInfoToRobot()
{
    bool timerIsActive = true;
    if(!initTimer->isActive()
            && !snReadTimer->isActive()
            && !snResultTimer->isActive()
            && !scanErrorTimer->isActive()
            && !testResultTimer->isActive()
            && !returnResultTimer->isActive()
            && !setRunModeTimer->isActive()
            )
    {
        timerIsActive = false;
    }

    if(!infoLineReadyTimer->isActive())
        infoLineReadyTimer->start(TIMEOUT_SEC+343);
    if((true==snCheckDoneState || true==scanDoneState) && (false==timerIsActive))
    {
        if(true == lineSensor1 && true == lineSensor2)
        {
            //告知Robot流水线已准备好
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Line ready"));

//            if(true==robotGetLineState)//for Branch1
//            {
//                robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Line ready"));
//                robotGetLineState = false;
//                if(infoLineReadyTimer->isActive())
//                    infoLineReadyTimer->stop();
//            }
        }
        else
        {
            //告知Robot流水线未准备好
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Line not ready"));
        }
    }
}

void RobotOnThread::ict_testTimeout()
{
//    robot_Init();//ICT测试超时，robot复位
    robot_stop();//ICT测试超时，断开Robot连接
}

void RobotOnThread::robot_readData(int clientID, QString IP, int Port, QString msg)
{
    //clientID = 0;
    emit forShow_To_Comm(forShowReceiveString(QString("%1 %2:%3").arg(IP).arg(Port).arg(msg)));
    if(0 <= msg.indexOf(QString("%1%2%3").arg(robotClient->prefix).arg("Starting program").arg(robotClient->suffix)))
    {
        if(robot_start_timer->isActive())
            robot_start_timer->stop();
        emit dashboard(1,tr("已启动"));
        emit dashboard(2,tr("暂停运行"));
        emit dashboard(3,tr("停止运行"));
        return;
    }
    if(0 <= msg.indexOf(QString("%1%2%3").arg(robotClient->prefix).arg("Pausing program").arg(robotClient->suffix)))
    {
        if(robot_pause_timer->isActive())
            robot_pause_timer->stop();
        emit dashboard(1,tr("启动"));
        emit dashboard(2,tr("已暂停"));
        emit dashboard(3,tr("停止运行"));
        return;
    }
    if(0 <= msg.indexOf(QString("%1%2%3").arg(robotClient->prefix).arg("Stopped").arg(robotClient->suffix)))
    {
        if(robot_stop_timer->isActive())
            robot_stop_timer->stop();
        emit dashboard(1,tr("启动"));
        emit dashboard(2,tr("暂停运行"));
        emit dashboard(3,tr("已停止"));
        return;
    }
    if(0 <= msg.indexOf(QString("%1Loading program: /programs/%2.urp%3").arg(robotClient->prefix).arg(robot_pro_num).arg(robotClient->suffix)))
    {
        if(setProtTimer->isActive())
            setProtTimer->stop();
        return;
    }
    if(0 <= msg.indexOf(QString("%1File not found: /programs/%2.urp%3").arg(robotClient->prefix).arg(robot_pro_num).arg(robotClient->suffix)))
    {
        if(setProtTimer->isActive())
            setProtTimer->stop();
        emit robot_Error_Msg(QString(tr("机器人加载程序%1.urp失败, 并未找到该程序!\n")).arg(robot_pro_num));
        return;
    }
}

void RobotOnThread::robot_start()
{
    if(robot_start_timer->isActive())
        robot_start_timer->stop();
    if(false == dashboard_enable)
    {
        return;
    }
    robot_start_timer->start(TIMEOUT_SEC+77);
    QString sendMsg = QString("%1%2%3").arg(robotClient->prefix).arg("play").arg(robotClient->suffix);
    robotClient->clientSendData(sendMsg);
    emit forShow_To_Comm(forShowSendString(sendMsg));
}

void RobotOnThread::robot_pause()
{
    if(robot_pause_timer->isActive())
        robot_pause_timer->stop();
    if(false == dashboard_enable)
    {
        return;
    }
    robot_pause_timer->start(TIMEOUT_SEC+33);
    QString sendMsg = QString("%1%2%3").arg(robotClient->prefix).arg("pause").arg(robotClient->suffix);
    robotClient->clientSendData(sendMsg);
    emit forShow_To_Comm(forShowSendString(sendMsg));
}

void RobotOnThread::robot_stop()
{
    if(robot_stop_timer->isActive())
        robot_stop_timer->stop();
    if(false == dashboard_enable)
    {
        return;
    }
    robot_stop_timer->start(TIMEOUT_SEC+11);
    QString sendMsg = QString("%1%2%3").arg(robotClient->prefix).arg("stop").arg(robotClient->suffix);
    robotClient->clientSendData(sendMsg);
    emit forShow_To_Comm(forShowSendString(sendMsg));
}

void RobotOnThread::setPro_Num(QString pro_num)
{
    if(""==pro_num)
    {
        return;
    }
    robot_pro_num = pro_num;
    setPro_Num_Timeout();
}

void RobotOnThread::setPro_Num_Timeout()
{
    if(setProtTimer->isActive())
        setProtTimer->stop();
    if(false == dashboard_enable)
    {
        return;
    }
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotTypeEnable = configRead->value(ROBOT_TYPE_ENABLE).toString();
    delete configRead;
    if("true"==robotTypeEnable)
    {
        setProtTimer->start(TIMEOUT_SEC);
        QString sendMsg = QString("%1load /programs/%2.urp%3").arg(robotClient->prefix).arg(robot_pro_num).arg(robotClient->suffix);
        robotClient->clientSendData(sendMsg);
        emit forShow_To_Comm(forShowSendString(sendMsg));
    }
}

void RobotOnThread::robot_clientDisConnect(int clientID, QString IP, int Port)
{
    //clientID = 0;
    //IP = "";
    //Port = 0;
    dashboard_enable = false;
    emit forShow_To_Comm(forShowString(QString(tr("Dashboard Server功能已停止!"))));
}

void RobotOnThread::cylinderDownTimerTimeout()
{
    if(cylinderDownTimer->isActive())
        cylinderDownTimer->stop();
    emit cylinderUpDown(CONTROL_OUT1_OFF);
}
