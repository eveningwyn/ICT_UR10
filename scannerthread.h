#ifndef SCANNERTHREAD_H
#define SCANNERTHREAD_H

#include <QThread>
#include <QTimer>

class ScannerThread : public QThread
{
    Q_OBJECT

public:
    explicit ScannerThread(QObject *parent = 0);
    void run();
    void stop();

signals:
    void scanResult(QString sn);//SN Msg
    void scanError(QString errorMsg);//errorMsg
    void forShow(QString msg);

private slots:
    void scannerScanSN();//start Scan
    void scannerReadSN();//read SN
    void timerTimeOut();//Scan timeout

private:
    QString prefix;
    QString suffix;
    QTimer *timer;
    int scanCount;//Scan count
    bool isScan;
    volatile bool stopped;
    QString forShowReceiveString(QString str);
    QString forShowSendString(QString str);
};

#endif // SCANNERTHREAD_H
