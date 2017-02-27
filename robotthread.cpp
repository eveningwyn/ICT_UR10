#include "robotthread.h"
#include <QDateTime>
#include <QSettings>
#include "ict_ur10.h"
#include <QDebug>

RobotThread::RobotThread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
}

void RobotThread::run()
{
    while (!stopped)
    {
//        qDebug()<<"RobotThread is running";
        msleep(10);
    }
    stopped = false;
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
    /*/////////////////////////////////////*/


    emit forShow(forShowReceiveString(QString("%1 %2:%3").arg(IP).arg(Port).arg(readMsg)));
}

void RobotThread::robotSendMsg(QString sendMsg)
{
    QSettings *configRead = new QSettings("..\\path/Config.ini", QSettings::IniFormat);
    quint16 port =(quint16) configRead->value("/RobotParameter/RobotPort").toString().toInt();
    delete configRead;

    ICT_UR10 *ptr = (ICT_UR10*)this->parent();
    ptr->robotServer->sendData(port,sendMsg);

    emit forShow(forShowSendString(sendMsg));
}

void RobotThread::robotManualSendMsg(QString sendMsg)
{
    robotSendMsg(sendMsg);
}
