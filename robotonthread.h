﻿#ifndef ROBOTONTHREAD_H
#define ROBOTONTHREAD_H

#include <QObject>
#include "tcpipserver.h"
#include <QTimer>
#include <QMutex>


class RobotOnThread : public QObject
{
    Q_OBJECT
public:
    explicit RobotOnThread(QObject *parent = 0);

private:
    TcpIpServer *robotServer;
    TcpIpClient *robotClient;
    bool robotPortExist;

    QString forShowReceiveString(QString str);
    QString forShowSendString(QString str);
    QString forShowString(QString str);

    void informationCheck(QString msg);
    QTimer *initTimer;
    QTimer *snReadTimer;
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
    bool robotAutoMode;
    QTimer *setRunModeTimer;
    volatile bool lineSensor1;
    volatile bool lineSensor2;
//    bool robotGetLineState;
    QTimer *infoLineReadyTimer;
    bool robotInitDone;

    QTimer *robot_start_timer;
    QTimer *robot_pause_timer;
    QTimer *robot_stop_timer;
    bool dashboard_enable;

    bool readedSN;
    bool scanDoneState;
    bool snCheckDoneState;

    QMutex robot_read_mutex;
    QMutex robot_send_mutex;

    QTimer *cylinderDownTimer;

signals:
    void robot_Status(QString status);//更新连接状态给主界面
    void robot_Error_Msg(QString errorMsg);//更新错误信号
    void robotConnected(QString IP,int Port);
    void robotDisconnected(QString IP,int Port);

    void startScan(bool autoScan);//触发scanner线程开始扫描
    void forShow_To_Comm(QString msg);//通讯信息显示
    void startTest();//ICT开始测试启动信号
    void sortComplete(bool testResultPass);//Robot分拣完成信号
    void robotReady(bool isReady);
    void recordTestResult(QString sn, QString result);//记录测试信息
    void setRunStatus(bool isAuto);//设置允许状态
    void cylinderUpDown(QString str);
    void robot_catchFail();
    void debugRunDone();
    void change_auto_debug_label(QString labelStr);
    void dashboard(int index,QString showStr);

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
    void serverSendError();
    void setRobotRunMode(bool autoMode);
    void setrobotPortExist(bool robot_exist);
    void debug_moveToScan();
    void debug_fixturePickup();
    void debug_fixturePlace();
    void debug_ictPlace();
    void debug_ictPickup();
    void debug_ictClose();
    void debug_ictOpen();
    void debug_placeOKPos();
    void debug_placeNGPos();
    void debug_returnSafePos();
    void debug_clawOpen();
    void debug_clawClose();
    void set_light_Red_Green_Yellow_Buzzer(QString msg);
    void lineSensorStatus(bool sensor1True,bool sensor2True);
    void ict_testTimeout();

    void robot_start();
    void robot_pause();
    void robot_stop();
    void robot_readData(int clientID,QString IP,int Port,QString msg);
    void robot_clientDisConnect(int clientID,QString IP,int Port);
    void readSnDoneTimeout();
    void readSnDone(bool readed);

private slots:
    void scanDone();
    void testDone();
    void setPro_Num_Timeout();//设置产品类型对应的程序号
    void setRunModeTimeout();
    void infromLineInfoToRobot();
    void cylinderDownTimerTimeout();
};

#endif // ROBOTONTHREAD_H
