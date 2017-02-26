#ifndef ICT_UR10_H
#define ICT_UR10_H

#include <QMainWindow>
#include "serialportwidget.h"
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
    ScannerThread *scan_thread;
    RobotThread *robot_thread;
    void manualStartScan();
    void disEnable();
    void Enable();
    bool commDlgIsShow;
    bool loginDlgIsShow;

signals:
    void sig();
    void manualScan();

private slots:
    void on_actionScanner_triggered();

    void on_actionRobot_triggered();

    void on_actionCommunication_triggered();

    void getSn(QString sn);

    void errorMessage(QString errorMsg);

    void on_actionLogin_triggered();

private:
    Ui::ICT_UR10 *ui;
    CommunicationDialog *commDlg;
    LoginDialog *loginDlg;
    void initialize();
    void closeEvent(QCloseEvent *event);
};

#endif // ICT_UR10_H
