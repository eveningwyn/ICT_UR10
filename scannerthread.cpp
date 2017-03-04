#include "scannerthread.h"
#include "ict_ur10.h"
#include <QMessageBox>
#include <QDateTime>

ScannerThread::ScannerThread(QObject *parent) :
    QThread(parent)
{
    prefix = "";
    suffix = "\r\n";
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTimeOut()));

    scanCount = 0;
    isScan = false;
    canRead = false;
    stopped = false;
}

void ScannerThread::run()
{
    while (!stopped)
    {
//        msleep(10);
    }
    stopped = false;
}

void ScannerThread::stop()
{
    stopped = true;
}

void ScannerThread::scannerReadSN()
{
    ICT_UR10 *ptr = (ICT_UR10*)this->parent();
    if(false == canRead)
    {
        ptr->scanner->clearBuffer();
        return;
    }
   QString sn;
   ptr->scanner->serialPortRead(sn,prefix,suffix);
   if(sn.isEmpty())
       return;
   if("noread"==sn.toLower())
   {

   }
   emit scanResult(sn);
   emit forShow(forShowReceiveString(sn));

   ptr->scanner->clearBuffer();
   timer->stop();
   isScan = false;
   canRead = false;
   scanCount = 0;
}

void ScannerThread::scannerScanSN()
{
    if(false == isScan)
    {
        canRead = true;
        ICT_UR10 *ptr = (ICT_UR10*)this->parent();
        ptr->scanner->clearBuffer();
        ptr->scanner->serialPortWrite(prefix+"<T>"+suffix);
        emit forShow(forShowSendString(prefix+"<T>"+suffix));
        isScan = true;
        timer->start(2200);
        scanCount++;
    }
}

void ScannerThread::timerTimeOut()
{
    if(timer->isActive())
    {
        timer->stop();
        isScan = false;
    }

//    //for debug
//    QString sn = "sn1234567890\r\n";
//    emit scanResult(sn);
//    emit forShow(forShowReceiveString(sn));
//    return;

    if(3 > scanCount)//3 times scanning
    {
        scannerScanSN();
        return;
    }

    scanCount = 0;

    emit scanError("Scan barcode timeout!\r\n");
//    emit forShow(forShowReceiveString(errorMsg));
}

QString ScannerThread::forShowReceiveString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Scanner_Receive:" + str;
    return str;
}

QString ScannerThread::forShowSendString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Scanner_Send:" + str;
    return str;
}
