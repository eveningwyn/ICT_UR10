#include "robotthread.h"
#include <QDateTime>
#include <QSettings>
#include "ict_ur10.h"
#include "staticname.h"

RobotThread::RobotThread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
}

void RobotThread::run()
{
    while (!stopped)
    {
//        msleep(10);
    }
//    stopped = false;
}

void RobotThread::stop()
{
    stopped = true;
}

QString RobotThread::forShowReceiveString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Robot_Receive:" + str + "\r\n";
    return str;
}

QString RobotThread::forShowSendString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Robot_Send:" + str + "\r\n";
    return str;
}

void RobotThread::robotReadData(QString IP, int Port, QString readMsg)
{
    emit forShow(forShowReceiveString(QString("%1 %2:%3").arg(IP).arg(Port).arg(readMsg)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP ==IP && robotPort==QString("%1").arg(Port))
    {
        informationCheck(readMsg);//根据协议处理接收的数据
    }
}

void RobotThread::robotSendMsg(QString sendMsg)
{
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;

    ICT_UR10 *ptr = (ICT_UR10*)this->parent();
    ptr->robotServer->sendData(robotPort.toInt(),sendMsg);

    emit forShow(forShowSendString(sendMsg));
}

void RobotThread::robotManualSendMsg(QString sendMsg)
{
    robotSendMsg(sendMsg);
}

void RobotThread::informationCheck(QString msg)//根据协议处理接收的数据
{
    if(""==msg)
    {
        return;
    }
}

void RobotThread::checkSn(QString sn)
{
    if(true)
    {
        emit checkSnPass(sn);
    }
    else
    {
        emit errorMessage("Please check this products test station information!\n");
    }
}
