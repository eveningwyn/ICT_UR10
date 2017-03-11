#ifndef ICT_TEST_OBJ_H
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

signals:
    void ict_Error_Msg(QString errorMsg);
    void ictTestResult(QString result);
    void ictCanOpen();

public slots:
    void getIctInfo(QString fileName,QString &readMsg);
    void setIctInfo(QString fileName, QString writeMsg);
    void init_ict();
    void statusReadTimeout();
};

#endif // ICT_TEST_OBJ_H
