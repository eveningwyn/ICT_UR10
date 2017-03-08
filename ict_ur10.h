#ifndef ICT_UR10_H
#define ICT_UR10_H

#include <QMainWindow>
#include "communicationdialog.h"
#include "scanneronthread.h"
#include "robotonthread.h"
#include "logindialog.h"
#include "errorlistdialog.h"
#include <QCloseEvent>
#include <QLabel>
#include <QThread>
#include <QTimer>

namespace Ui {
class ICT_UR10;
}

class ICT_UR10 : public QMainWindow
{
    Q_OBJECT

public:
    explicit ICT_UR10(QWidget *parent = 0);
    ~ICT_UR10();
    QThread *scanThread;//scan子线程
    ScannerOnThread *scan_on_thread;//scan处理类-->移交到子线程运行
    void manualStartScan();

    QThread *robotThread;//robot子线程
    RobotOnThread *robot_on_thread;//robot处理类-->移交到子线程运行
    void manualSendMsg_robot(QString sendMsg);

    void disEnableUI();
    void Enable();
    bool commDlgIsShow;
    bool loginDlgIsShow;
    bool errorDlgIsShow;
    void updateTestResult(QString sn,QString result);
    int testCount;//测试次数，用于对主界面的计数

private:
    Ui::ICT_UR10 *ui;
    QTimer *scantimer;//扫描定时
    int scanCount;//扫描次数

    /*对话框*/
    CommunicationDialog *commDlg;
    LoginDialog *loginDlg;
    ErrorListDialog *errorDlg;

    /*状态栏*/
    QLabel *statusBarLabel_Scanner;
    QLabel *statusBarLabel_Robot;
    QLabel *statusBarLabel_ICT;

    void init_UI();//初始UI
    void newFile();//创建数据文件夹
    void closeEvent(QCloseEvent *event);//重载close事件
    QString forShowString(QString str);//将字符串添加时间戳

    /*生产看板*/
    int totalQty;//测试总数量
    int passQty;//Pass数量
    int failQty;//Fail数量
    float yield;//良率
    void update_UI_show();//更新看板数据

signals:
    void init_scanner_robot();//启动两个子线程的初始化
    void manualScan();//手动触发扫描
    void setCanScan();//设置扫描运行

    void manualSendMsg(QString sendMsg);//手动触发发送

    void forShow(QString msg);//显示信息给对话框
    void sendErrorMsg(QString msg);//错误记录显示

public slots:
    void update_Scanner_Status(QString status);//更新串口连接状态
    void start_scanner_timer();//告知主线程开始扫描超时计时
    void stop_scanner_timer();//告知主线程停止扫描超时计时

    void update_Robot_Status(QString status);//更新robot连接状态

private slots:
    void on_actionScanner_triggered();

    void on_actionRobot_triggered();

    void on_actionCommunication_triggered();

    void on_actionLogin_triggered();

    void on_actionError_list_triggered();

    void getSn(QString sn);

    void errorMessage(QString errorMsg);//弹框提示操作员

    void robotConnected(QString IP,int Port);

    void robotDisconnected(QString IP,int Port);

    void timerTimeOut();//扫描超时处理

    void on_pushButton_clicked();//调试用

    void on_pushButton_2_clicked();//调试用

};

#endif // ICT_UR10_H
