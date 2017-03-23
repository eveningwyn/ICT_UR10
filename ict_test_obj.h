﻿#ifndef ICT_TEST_OBJ_H
#define ICT_TEST_OBJ_H

#include <QObject>
#include <QTimer>

class ICT_Test_Obj : public QObject
{
    Q_OBJECT
public:
    explicit ICT_Test_Obj(QObject *parent = 0);

private:
    int pc_ict_Ping();
    QTimer *statusReadTimer;
    int count;
    bool ictEnable;
    QString ictStatusStr;
    QString ictEnableStr;
    QString snTemp;

signals:
    void ict_Error_Msg(QString errorMsg);
    void ictTestResult(QString result);
    void ictIsReady(bool isReady);
    void ict_Status(QString status);//更新连接状态给主界面
    void ict_Check_SN_Result(QString sn, bool checkPass);

public slots:
    void getIctInfo(QString fileName,QString &readMsg);
    void setIctInfo(QString fileName, QString writeMsg);
    void init_ict();
    void statusReadTimeout();
    void openTimer();
    void testStart();
    void set_ictEnable(bool enable);
    void ict_Check_SN(QString sn);
};

#endif // ICT_TEST_OBJ_H
