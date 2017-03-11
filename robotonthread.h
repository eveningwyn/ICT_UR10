#ifndef ROBOTONTHREAD_H
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
    QTimer *testResultTimer;
    QTimer *returnResultTimer;
    bool PC_Is_Ready;
    bool checkPass;

signals:
    void robot_Status(QString status);//更新连接状态给主界面
    void robot_Error_Msg(QString errorMsg);//更新错误信号
    void robotConnected(QString IP,int Port);
    void robotDisconnected(QString IP,int Port);

    void startScan();//触发scanner线程开始扫描
    void forShow_To_Comm(QString msg);//通讯信息显示
    void checkSnResult(QString sn,bool checkResult);//MES系统检测PASS信号
    void startTest();//ICT开始测试启动信号
    void sortComplete();//Robot分拣完成信号

public slots:
    void init_Robot();//初始化服务器连接
    void robotReadData(QString IP,int Port,QString readMsg);//读取客户端信息
    void robotSendMsg(QString sendMsg);//向客户端发送信息
    void checkSn(QString sn);//将SN进行MES校验
    void robot_Init();
    void snCheckResult(QString sn,bool checkResult);

private slots:
    void scanDone();
};

#endif // ROBOTONTHREAD_H
