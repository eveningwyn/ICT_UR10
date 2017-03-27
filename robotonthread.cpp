#include "robotonthread.h"
#include <QDateTime>
#include <QSettings>
#include "staticname.h"
#include "language.h"

#include <QThread>

#define TIMEOUT_SEC  500

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
    emit forShow_To_Comm(forShowReceiveString(QString("%1 %2:%3").arg(IP).arg(Port).arg(readMsg)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP ==IP && robotPort==QString("%1").arg(Port))
    {
        informationCheck(readMsg);//根据协议处理接收的数据
    }
}

void RobotOnThread::robotSendMsg(QString sendMsg)
{
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
            emit cylinderUpDown(CONTROL_OUT1_ON);
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
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Scan done ACK")))
        {
            if(snResultTimer->isActive())
                snResultTimer->stop();
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Scan error ACK")))
        {
            if(scanErrorTimer->isActive())
                scanErrorTimer->stop();
            emit setRunStatus(false);
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Test ready")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Test ready ACK"));
            emit startTest();//发出开始测试信号
            if(false == ictEnable)
            {
                QThread::msleep(2000);
                testResult("PASS");
            }
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
                emit cylinderUpDown(CONTROL_OUT1_OFF);
            }
            checkPass = false;
            barcode = "";
            testPass = false;
            emit setRunStatus(false);
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Sort complete ACK"));
            emit sortComplete();//发出分拣完成信号
            return;
        }
    }
    else//手动模式
    {
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Move to Scan done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Move to Scan done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Fixture pickup done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fixture pickup done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Fixture place done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fixture place done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT place done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT place done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT pickup done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT pickup done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT close done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT close done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("ICT open done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("ICT open done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Place OK done")))
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Place OK done ACK"));
            return;
        }
        if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Place NG done")))
        {
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
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg(robot_pro_num+" ACK")))
    {
        if(setProtTimer->isActive())
            setProtTimer->stop();
        return;
    }
    if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Auto mode ACK"))) ||
            (0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Debug mode ACK"))))
    {
        if(setRunModeTimer->isActive())
            setRunModeTimer->stop();
        return;
    }
    if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Auto mode ACK"))) ||
            (0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Debug mode ACK"))))
    {
        if(setRunModeTimer->isActive())
            setRunModeTimer->stop();
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
    snResultTimer = new QTimer(this);
    scanErrorTimer = new QTimer(this);
    testResultTimer = new QTimer(this);
    returnResultTimer = new QTimer(this);
    setProtTimer = new QTimer(this);
    setRunModeTimer = new QTimer(this);
    infoLineReadyTimer = new QTimer(this);

    PC_Is_Ready = false;
    checkPass = false;
    barcode = "";
    testPass = false;
    robot_pro_num = "";
    ictEnable = true;
    robotAutoMode = true;//初始化为自动模式
    robotPortExist = false;

    lineCanPlace = false;
    lineIsNoBoard = true;

    robotServer = new TcpIpServer(this);
    robotServer->set_prefix_suffix(PREFIX,SUFFIX);

    connect(robotServer,&TcpIpServer::errorMessage,this,&RobotOnThread::robot_Error_Msg);
    connect(robotServer,&TcpIpServer::clientConnect,this,&RobotOnThread::robotConnected);
    connect(robotServer,&TcpIpServer::clientDisconnected,this,&RobotOnThread::robotDisconnected);
    connect(robotServer,&TcpIpServer::serverReadData,this,&RobotOnThread::robotReadData);

    connect(initTimer,&QTimer::timeout,this,&RobotOnThread::robot_Init);
    connect(snResultTimer,&QTimer::timeout,this,&RobotOnThread::scanDone);
    connect(scanErrorTimer,&QTimer::timeout,this,&RobotOnThread::scanError);
    connect(testResultTimer,&QTimer::timeout,this,&RobotOnThread::testDone);
    connect(returnResultTimer,&QTimer::timeout,this,&RobotOnThread::roborReturn);
    connect(setProtTimer,&QTimer::timeout,this,&RobotOnThread::setPro_Num_Timeout);
    connect(setRunModeTimer,&QTimer::timeout,this,&RobotOnThread::setRunModeTimeout);

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    //Robot
    QString ipAddress =configRead->value(SERVER_IP_ADDRESS).toString();
    quint16 port =(quint16) configRead->value(SERVER_PORT).toString().toInt();

    if(!robotServer->stratListen(ipAddress,port))
    {
        emit robot_Status(tr("机器人:已断开"));
        emit robot_Error_Msg(tr("服务器创建失败,请检查后重启软件！\n"));
    }
    else
    {
        emit robot_Status(tr("机器人:服务器 Listening..."));
        emit forShow_To_Comm(forShowString(QString(tr("服务器 %1 %2 Listening..."))
                                           .arg(ipAddress).arg(port)));
    }
    delete configRead;
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
        snResultTimer->start(TIMEOUT_SEC);
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan done"));
    }
    else
    {
        barcode = "";
        scanError();
        return;
    }
}

void RobotOnThread::scanError()
{
    if(scanErrorTimer->isActive())
        scanErrorTimer->stop();
    scanErrorTimer->start(TIMEOUT_SEC);
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan error"));
}

void RobotOnThread::testResult(QString result)
{
    if("PASS"==result)
    {
        testPass = true;
        emit recordTestResult(barcode,"PASS");
    }
    else
        if("FAIL"==result)
        {
            testPass = false;
            emit recordTestResult(barcode,"FAIL");
        }
    testDone();
}

void RobotOnThread::testDone()
{
    if(testResultTimer->isActive())
        testResultTimer->stop();
    testResultTimer->start(TIMEOUT_SEC);
    if(true == testPass)
    {
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Pass done"));
    }
    else
    {
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Fail done"));
    }
}

void RobotOnThread::roborReturn()
{
    if(returnResultTimer->isActive())
        returnResultTimer->stop();
    returnResultTimer->start(TIMEOUT_SEC);
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot return"));
}

void RobotOnThread::set_PC_Status(bool isReady)
{
    PC_Is_Ready = isReady;
}

void RobotOnThread::setPro_Num(QString pro_num)
{
    robot_pro_num = pro_num;
    setPro_Num_Timeout();
}

void RobotOnThread::setPro_Num_Timeout()
{
    if(setProtTimer->isActive())
        setProtTimer->stop();
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotTypeEnable = configRead->value(ROBOT_TYPE_ENABLE).toString();
    if("true"==robotTypeEnable)
    {
        setProtTimer->start(TIMEOUT_SEC);
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg(robot_pro_num));
    }
    delete configRead;
}

void RobotOnThread::set_ictEnable(bool enable)
{
    ictEnable = enable;
}

void RobotOnThread::serverSendError()
{
    initTimer->stop();
    snResultTimer->stop();
    scanErrorTimer->stop();
    testResultTimer->stop();
    returnResultTimer->stop();
    setProtTimer->stop();
    setRunModeTimer->stop();
}

void RobotOnThread::setRunModeTimeout()
{
    if(setRunModeTimer->isActive())
        setRunModeTimer->stop();
    setRobotRunMode(robotAutoMode);
}

void RobotOnThread::setRobotRunMode(bool autoMode)
{
    bool modeTemp = robotAutoMode;
    robotAutoMode = autoMode;
    if(false == modeTemp && true == robotAutoMode)
    {
        emit robot_Error_Msg(tr("UR机器人需要复位，请注意人员和设备安全！\n"));
        robot_Init();//Debug模式切换到Auto模式后，Robot需复位
    }
    setRunModeTimer->start(TIMEOUT_SEC);
    if(true == robotInitDone)
    {
        if(true == robotAutoMode)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Auto mode"));
        }
        else
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Debug mode"));
        }
    }
}

void RobotOnThread::setrobotPortExist(bool robot_exist)
{
    robotPortExist = robot_exist;
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

void RobotOnThread::set_light_Red_Green_Yellow_Buzzer(QString msg)
{
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg(msg));
}

void RobotOnThread::lineReadyStatus(bool isTrue)
{
    bool temp = lineCanPlace;
    lineCanPlace = isTrue;
    if(temp != lineCanPlace)
    {
        infromLineInfoToRobot();
    }
}

void RobotOnThread::lineNoBoardStatus(bool isTrue)
{
    bool temp = lineCanPlace;
    bool temp1 = lineIsNoBoard;
    lineIsNoBoard = isTrue;
    lineCanPlace = false;
    if(temp != lineCanPlace)
    {
        infromLineInfoToRobot();
    }
    if(temp1 != lineIsNoBoard && true == lineIsNoBoard)
    {
        emit cylinderUpDown(CONTROL_OUT1_ON);
    }
}

void RobotOnThread::infromLineInfoToRobot()
{
    if(infoLineReadyTimer->isActive())
        infoLineReadyTimer->stop();
    infoLineReadyTimer->start(TIMEOUT_SEC);
    if(true == lineCanPlace)
    {
        //告知Robot流水线已准备好
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Line ready"));
    }
    else
    {
        //告知Robot流水线未准备好
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Line not ready"));
    }
}

void RobotOnThread::ict_testTimeout()
{
    robot_Init();//ICT测试超时，robot复位
}
