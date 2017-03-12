#ifndef MESCHECKOBJ_H
#define MESCHECKOBJ_H

#include <QObject>

class MesCheckObj : public QObject
{
    Q_OBJECT
public:
    explicit MesCheckObj(QObject *parent = 0);

private:
    bool mesEnable;

signals:
    void checkSnResult(QString sn,bool checkResult);//MES系统检测PASS信号
    void mes_Error_Msg(QString errorMsg);//更新错误信号

public slots:
    void init_mes();
    void checkSn(QString sn);//将SN进行MES校验
    void set_mes_enable(bool enable);
};

#endif // MESCHECKOBJ_H
