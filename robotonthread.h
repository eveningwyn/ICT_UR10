﻿#ifndef ROBOTONTHREAD_H
#define ROBOTONTHREAD_H

#include <QObject>
#include "tcpipserver.h"
#include <QTimer>


class RobotOnThread : public QObject
{
    Q_OBJECT
public:
    explicit RobotOnThread(QObject *parent = 0);

private:
    TcpIpServer *robotServer;

    QString forShowReceiveString(QString str);
    QString forShowSendString(QString str);
    QString forShowString(QString str);

    void informationCheck(QString msg);
    QTimer *initTimer;
    QTimer *snResultTimer;
    QTimer *scanErrorTimer;
    QTimer *testResultTimer;
    QTimer *returnResultTimer;
    QTimer *setProtTimer;
    bool PC_Is_Ready;
    bool checkPass;
    QString barcode;
    bool testPass;
    QString robot_pro_num;

signals:
    void robot_Status(QString status);//更新连接状态给主界面
    void robot_Error_Msg(QString errorMsg);//更新错误信号
    void robotConnected(QString IP,int Port);
    void robotDisconnected(QString IP,int Port);

    void startScan();//触发scanner线程开始扫描
    void forShow_To_Comm(QString msg);//通讯信息显示
    void startTest(QString sn);//ICT开始测试启动信号
    void sortComplete();//Robot分拣完成信号
    void robotReady(bool isReady);
    void recordTestResult(QString sn, QString result);//记录测试信息
    void setRunStatus(bool isAuto);//设置允许状态

public slots:
    void init_Robot();//初始化服务器连接
    void robotReadData(QString IP,int Port,QString readMsg);//读取客户端信息
    void robotSendMsg(QString sendMsg);//向客户端发送信息
    void robot_Init();
    void snCheckResult(QString sn,bool checkResult);
    void testResult(QString result);
    void set_PC_Status(bool isReady);
    void scanError();
    void roborReturn();
    void setPro_Num(QString pro_num);//设置产品类型对应的程序号

private slots:
    void scanDone();
    void testDone();
    void setPro_Num_Timeout();//设置产品类型对应的程序号
};

#endif // ROBOTONTHREAD_H
