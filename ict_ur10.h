#ifndef ICT_UR10_H
#define ICT_UR10_H

#include <QMainWindow>
#include "communicationdialog.h"
#include "logindialog.h"
#include "errorlistdialog.h"
#include "ict_mes_dialog.h"
#include "scanneronthread.h"
#include "robotonthread.h"
#include <QCloseEvent>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include "ict_test_obj.h"
#include "mescheckobj.h"

namespace Ui {
class ICT_UR10;
}

class ICT_UR10 : public QMainWindow
{
    Q_OBJECT

public:
    explicit ICT_UR10(QWidget *parent = 0);
    ~ICT_UR10();
    QThread *thread1;//thread1子线程
    ScannerOnThread *scan_on_thread;//scan处理类-->移交到子线程运行
    void manualStartScan(bool autoScan);

    QThread *thread2;//thread2子线程
    RobotOnThread *robot_on_thread;//robot处理类-->移交到子线程运行
    void manualSendMsg_robot(QString sendMsg);

    QThread *thread3;//thread3子线程
    ICT_Test_Obj *ict;

    void disEnableUI();
    void Enable();
    bool commDlgIsShow;
    bool loginDlgIsShow;
    bool errorDlgIsShow;
    void updateTestResult(QString sn,QString result);
    int testCount;//测试次数，用于对主界面的计数
    bool ictEnable;
    bool isAutoRun;

private:
    Ui::ICT_UR10 *ui;
    bool robotIsInit;

    bool scannerIsReady;
    bool robotIsReady;
    bool ictIsReady;
    bool mainInitDone;

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
    int failCount;//连续fail计数
    int totalQtyTemp;
    int passQtyTemp;//Pass数量
    int failQtyTemp;//Fail数量
    float yieldTemp;//良率
    int lightCount;//0表示三色灯没有工作，1表示红灯点亮，2表示绿灯点亮，3表示黄灯点亮，4表示蜂鸣器工作

signals:
    void init_scanner_robot_ict_mes();//启动子线程的初始化
    void manualScan(bool autoScan);//手动触发扫描

    void manualSendMsg(QString sendMsg);//手动触发发送

    void forShow(QString msg);//显示信息给对话框
    void sendErrorMsg(QString msg);//错误记录显示

    void robotInit();
    void pcIsReady(bool isReady);
    void setType_Pro(QString pro_num);//设置产品类型对应的程序号
    void set_ict_Enable(bool enable);
    void robotSetAutoMode(bool autoMode);
    void robotPortExist(bool robot_exist);
    void manualSendMsg_controlBoard(QString sendMsg);
    void light_Red_Green_Yellow_Buzzer(QString str);

public slots:
    void update_Scanner_Status(QString status);//更新串口连接状态
    void update_Robot_Status(QString status);//更新robot连接状态
    void update_ICT_Status(QString status);//更新robot连接状态
    void robotInitStatus(bool status);
    void PC_Status();
    void setScannerReady(bool isReady);
    void setRobotReady(bool isReady);
    void setIctReady(bool isReady);
    void runStatus(bool isAuto);

private slots:
    void on_actionScanner_triggered();

    void on_actionRobot_triggered();

    void on_actionCommunication_triggered();

    void on_actionLogin_triggered();

    void on_actionError_list_triggered();

    void getSn(QString sn,bool checkResult);

    void showErrorMessage(QString errorMsg);//弹框提示操作员

    void robotConnected(QString IP,int Port);

    void robotDisconnected(QString IP,int Port);

    void on_comboBoxTypeSelect_currentTextChanged(const QString &arg1);
    void on_actionICT_MES_triggered();
    void on_actionDebug_triggered();
};

#endif // ICT_UR10_H
