#include "robotthread.h"
#include <QDateTime>
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
//        msleep(10);
    }
    stopped = false;
}

void RobotThread::stop()
{
    stopped = true;
}

QString RobotThread::forShowReceiveString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Scanner_Receive:" + str;
    return str;
}

QString RobotThread::forShowSendString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Scanner_Send:" + str;
    return str;
}
