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

signals:
    void startScan();

private:
    volatile bool stopped;
    QString forShowReceiveString(QString str);
    QString forShowSendString(QString str);
};

#endif // ROBOTTHREAD_H
