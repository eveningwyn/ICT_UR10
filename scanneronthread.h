#ifndef SCANNERONTHREAD_H
#define SCANNERONTHREAD_H

#include <QObject>
#include "serialportobj.h"

class ScannerOnThread : public QObject
{
    Q_OBJECT

public:
    explicit ScannerOnThread(QObject *parent = 0);

private:
    SerialPortObj *scanner;
    QString prefix;
    QString suffix;
    bool canScan;
    bool canRead;
    QString forShowReceiveString(QString str);
    QString forShowSendString(QString str);
    QString forShowString(QString str);

signals:
    void scanResult(QString sn);//传送SN信息
    void forShow_To_Comm(QString msg);//信息记录
    void scanner_Error_Msg(QString errorMsg);//发送错误信息errorMsg
    void scanner_Status(QString status);//更新连接状态给主界面
    void start_timer();//告知主线程开始扫描超时计时
    void stop_timer();//告知主线程停止扫描超时计时

public slots:
    void init_Scanner();//初始化串口
    void scannerScanSN();//启动扫描
    void scannerReadSN();//读取SN
    void setCanScan();//设置可以触发扫描

private slots:

};

#endif // SCANNERONTHREAD_H
