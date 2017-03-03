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
    void updateScannerStatue(QString portName, bool connected);
    void updateTestResult(QString sn,QString result);
    int testCount;

signals:
    void manualScan();
    void forShow(QString msg);

private slots:
    void on_actionScanner_triggered();

    void on_actionRobot_triggered();

    void on_actionCommunication_triggered();

    void getSn(QString sn);

    void errorMessage(QString errorMsg);

    void on_actionLogin_triggered();

    void robotConnected(QString IP,int Port);

    void robotDisconnected(QString IP,int Port);

    void on_pushButton_clicked();

private:
    Ui::ICT_UR10 *ui;
    CommunicationDialog *commDlg;
    LoginDialog *loginDlg;
    void init_Scanner_Robot();
    void closeEvent(QCloseEvent *event);
    QString forShowString(QString str);
    void newFile();
};

#endif // ICT_UR10_H
