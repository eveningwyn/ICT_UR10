#include "robotonthread.h"
#include <QDateTime>
#include <QSettings>
#include "staticname.h"

#include <QDebug>

RobotOnThread::RobotOnThread(QObject *parent) : QObject(parent)
{
}

QString RobotOnThread::forShowReceiveString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Robot_Receive:" + str + "\r\n";
    return str;
}

QString RobotOnThread::forShowSendString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Robot_Send:" + str + "\r\n";
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

    robotServer->sendData(robotPort.toInt(),sendMsg);//出现了线程阻塞，待排查

    emit forShow_To_Comm(forShowSendString(sendMsg));
}

void RobotOnThread::informationCheck(QString msg)//根据协议处理接收的数据
{
    if(""==msg)
    {
        return;
    }
}

void RobotOnThread::checkSn(QString sn)
{
    if(true)
    {
        emit checkSnPass(sn);
    }
    else
    {
        emit robot_Error_Msg("Please check this products test station information!\n");
    }
}

void RobotOnThread::init_Robot()
{
    robotServer = new TcpIpServer(this);
    robotServer->set_prefix_suffix("","\r\n");

    connect(robotServer,&TcpIpServer::errorMessage,this,&RobotOnThread::robot_Error_Msg);
    connect(robotServer,&TcpIpServer::clientConnect,this,&RobotOnThread::robotConnected);
    connect(robotServer,&TcpIpServer::clientDisconnected,this,&RobotOnThread::robotDisconnected);
    connect(robotServer,&TcpIpServer::serverReadData,this,&RobotOnThread::robotReadData);

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    //Robot
    QString ipAddress =configRead->value(SERVER_IP_ADDRESS).toString();
    quint16 port =(quint16) configRead->value(SERVER_PORT).toString().toInt();
    if(!robotServer->stratListen(ipAddress,port))
    {
        emit robot_Status("Robot:Disconnected!");
        emit robot_Error_Msg("Robot initialize failed!\n");
    }
    else
    {
        emit robot_Status("Robot:Listening...");
        emit forShow_To_Comm(forShowString(QString("Robot:%1 %2 Listening...")
                                           .arg(ipAddress).arg(port)));
    }
    delete configRead;
}
