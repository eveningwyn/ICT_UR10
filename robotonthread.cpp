#include "robotonthread.h"
#include <QDateTime>
#include <QSettings>
#include "staticname.h"
#include "language.h"

#include <QThread>
#include <QDebug>

RobotOnThread::RobotOnThread(QObject *parent) : QObject(parent)
{
}

QString RobotOnThread::forShowReceiveString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Receive_from_Robot:" + str + "\r\n";
    return str;
}

QString RobotOnThread::forShowSendString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Send_to_Robot:" + str + "\r\n";
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
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;

    robotServer->sendData(robotPort.toInt(),sendMsg);

    emit forShow_To_Comm(forShowSendString(sendMsg));
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
        emit robot_Status(tr("机器人:初始化完成"));
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot init done ACK"));
        return;
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("PC status?")))
    {
        if(true == PC_Is_Ready)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("PC ready"));
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
        if(true == PC_Is_Ready)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan ready ACK"));
            emit startScan();
            return;
        }
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Scan done ACK")))
    {
        if(snResultTimer->isActive())
            snResultTimer->stop();
        return;
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Test ready")))
    {
        if(true == PC_Is_Ready)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Test ready ACK"));
            emit startTest();//发出开始测试信号
            return;
        }
    }
    if((0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Pass done ACK")))||
            (0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Fail done ACK"))))
    {
        if(testResultTimer->isActive())
            testResultTimer->stop();
        return;
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Sort complete")))
    {
        if(true == PC_Is_Ready)
        {
            robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Sort complete ACK"));
            emit sortComplete();//发出分拣完成信号
            return;
        }
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Error")))
    {
        if(true == PC_Is_Ready)
        {
            msg.replace(SUFFIX,"");
            robotSendMsg(QString("%1 ACK%2").arg(msg).arg(SUFFIX));
            emit robot_Error_Msg(msg+SUFFIX+"\n");//发出报错信息
            return;
        }
    }
    if(0 <= msg.indexOf(QString(PREFIX_COMMAND).arg("Robot return ACK")))
    {
        if(returnResultTimer->isActive())
            returnResultTimer->stop();
        return;
    }
}

void RobotOnThread::checkSn(QString sn)
{
    if(true)
    {
        emit checkSnResult(sn,true);
    }
    else
    {
        emit checkSnResult(sn,false);
        emit robot_Error_Msg(tr("当前产品不属于当前工站，请确认该产品已正常过站！\n"));
    }
}

void RobotOnThread::init_Robot()
{
    initTimer = new QTimer(this);
    snResultTimer = new QTimer(this);
    testResultTimer = new QTimer(this);//未关联timeout
    returnResultTimer = new QTimer(this);//未关联timeout
    PC_Is_Ready = true;//此处初始化为true，是为了调试使用，实际状态由ICT的状态决定

    robotServer = new TcpIpServer(this);
    robotServer->set_prefix_suffix(PREFIX,SUFFIX);

    connect(robotServer,&TcpIpServer::errorMessage,this,&RobotOnThread::robot_Error_Msg);
    connect(robotServer,&TcpIpServer::clientConnect,this,&RobotOnThread::robotConnected);
    connect(robotServer,&TcpIpServer::clientDisconnected,this,&RobotOnThread::robotDisconnected);
    connect(robotServer,&TcpIpServer::serverReadData,this,&RobotOnThread::robotReadData);

    connect(initTimer,&QTimer::timeout,this,&RobotOnThread::robot_Init);
    connect(snResultTimer,&QTimer::timeout,this,&RobotOnThread::scanDone);

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

/*通讯协议处理部分*/
void RobotOnThread::robot_Init()
{
    if(initTimer->isActive())
        initTimer->stop();
    robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Robot init"));
    initTimer->start(5000);
}

void RobotOnThread::snCheckResult(QString sn,bool checkResult)
{
    checkPass = checkResult;
    scanDone();
}

void RobotOnThread::scanDone()
{
    if(snResultTimer->isActive())
        snResultTimer->stop();
    if(true == checkPass)
    {
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan done"));
    }
    else
    {
        robotSendMsg(QString(PREFIX_COMMAND_SUFFIX).arg("Scan done"));
    }
    snResultTimer->start(5000);
}
