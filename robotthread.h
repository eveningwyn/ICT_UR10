#ifndef ROBOTTHREAD_H
#define ROBOTTHREAD_H

#include <QThread>

class RobotThread : public QThread
{
    Q_OBJECT

public:
    explicit RobotThread(QObject *parent = 0);
    void run();
    void stop();
    void robotManualSendMsg(QString sendMsg);

signals:
    void startScan();
    void forShow(QString msg);

private slots:
    void robotReadData(QString IP,int Port,QString readMsg);
    void robotSendMsg(QString sendMsg);

private:
    volatile bool stopped;
    QString forShowReceiveString(QString str);
    QString forShowSendString(QString str);
    void informationCheck(QString msg);
};

#endif // ROBOTTHREAD_H
