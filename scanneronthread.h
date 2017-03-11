﻿#ifndef SCANNERONTHREAD_H
#define SCANNERONTHREAD_H

#include <QObject>
#include "serialportobj.h"
#include <QTimer>

class ScannerOnThread : public QObject
{
    Q_OBJECT

public:
    explicit ScannerOnThread(QObject *parent = 0);

private:
    SerialPortObj *scanner;
    QTimer *scantimer;
    int scanCount;//扫描次数
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

public slots:
    void init_Scanner();//初始化串口
    void scannerScanSN();//启动扫描
    void scannerReadSN();//读取SN

private slots:
    void timerTimeOut();//扫描超时处理
};

#endif // SCANNERONTHREAD_H
