﻿#ifndef MESCHECKOBJ_H
#define MESCHECKOBJ_H

#include <QObject>
#include <QtNetwork>

class MesCheckObj : public QObject
{
    Q_OBJECT
public:
    explicit MesCheckObj(QObject *parent = 0);

private:
    bool mesEnable;
    QNetworkAccessManager *mes_manager;
    QNetworkRequest mes_request;
    QNetworkReply *mes_reply;
    QString V_UID;
    QString V_USER_ID;
    void get_mes_SN_info(QString sn);

signals:
    void checkSnResult(QString sn,bool checkResult);//MES系统检测PASS信号
    void mes_Error_Msg(QString errorMsg);//更新错误信号

public slots:
    void init_mes();
    void checkSn(QString sn);//将SN进行MES校验
    void set_mes_enable(bool enable);
    void get_mes_config();
    void post_mes(QString V_UID, QString V_USER_ID, QString V_LIBRARY_ACTION_CODE, QString V_INPUT);

private slots:
    void replyFinished(QNetworkReply *reply);

};

#endif // MESCHECKOBJ_H