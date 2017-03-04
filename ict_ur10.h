#ifndef ICT_UR10_H
#define ICT_UR10_H

#include <QMainWindow>
#include "serialportwidget.h"
#include "tcpipserver.h"
#include "communicationdialog.h"
#include "scannerthread.h"
#include "robotthread.h"
#include "logindialog.h"
#include <QCloseEvent>
#include <QLabel>
#include "errorlistdialog.h"

namespace Ui {
class ICT_UR10;
}

class ICT_UR10 : public QMainWindow
{
    Q_OBJECT

public:
    explicit ICT_UR10(QWidget *parent = 0);
    ~ICT_UR10();

    SerialPortWidget *scanner;
    TcpIpServer *robotServer;
    ScannerThread *scan_thread;
    RobotThread *robot_thread;
    void manualStartScan();
    void disEnableUI();
    void Enable();
    bool commDlgIsShow;
    bool loginDlgIsShow;
    bool errorDlgIsShow;
    void updateScannerStatue(QString portName, bool connected);
    void updateTestResult(QString sn,QString result);
    int testCount;

signals:
    void manualScan();
    void forShow(QString msg);
    void sendErrorMsg(QString msg);//错误记录显示

private slots:
    void on_actionScanner_triggered();

    void on_actionRobot_triggered();

    void on_actionCommunication_triggered();

    void getSn(QString sn);

    void errorMessage(QString errorMsg);//弹框提示操作员

    void on_actionLogin_triggered();

    void robotConnected(QString IP,int Port);

    void robotDisconnected(QString IP,int Port);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionError_list_triggered();

private:
    Ui::ICT_UR10 *ui;
    CommunicationDialog *commDlg;
    LoginDialog *loginDlg;
    ErrorListDialog *errorDlg;
    QLabel *statusBarLabel_Scanner;
    QLabel *statusBarLabel_Robot;
    void init_Scanner_Robot();
    void closeEvent(QCloseEvent *event);
    QString forShowString(QString str);
    void newFile();

    /*生产看板*/
    int totalQty;
    int passQty;
    int failQty;
    float yield;
    void update_UI_show();

};

#endif // ICT_UR10_H
