#include "ict_test_obj.h"
#include "staticname.h"
#include "language.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QProcess>
#include <QDateTime>
#include <QDebug>

ICT_Test_Obj::ICT_Test_Obj(QObject *parent) : QObject(parent)
{

}

void ICT_Test_Obj::getIctInfo(QString fileName, QString &readMsg)
{
    QFile file(QString(ICT_LOCAL_FILE_PATH).arg(fileName));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(QString(tr("打开ICT测试机的本地文件<%1>失败!\n")).arg(fileName));
        return ;
    }
    QTextStream in_out(&file);
    readMsg = in_out.readAll();
    file.close();
    //存在有效信息则在读取之后清空文件
    if(!readMsg.isEmpty())
    {
        setIctInfo(fileName,"");
    }
    return;
}

void ICT_Test_Obj::setIctInfo(QString fileName, QString writeMsg)
{
    QFile file(QString(ICT_LOCAL_FILE_PATH).arg(fileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(QString(tr("打开ICT测试机的本地文件<%1>失败!\n")).arg(fileName));
        return ;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);//鼠标指针变为等待状态
    out << writeMsg;
    QApplication::restoreOverrideCursor();//鼠标指针恢复原来的状态
    file.close();
    return;
}

int ICT_Test_Obj::pc_ict_Ping()
{
    QString ip = ICT_LOCAL_IP;
    QString pingStr = "ping " + ip + " -n 1";
    int ref = QProcess::execute(pingStr);
    return ref;
}

void ICT_Test_Obj::init_ict()
{
    count = 0;
    result = "";
    canOpen = false;
    statusReadTimer = new QTimer(this);
    connect(statusReadTimer,&QTimer::timeout,this,&ICT_Test_Obj::statusReadTimeout);
    statusReadTimer->start(500);
}

void ICT_Test_Obj::statusReadTimeout()
{
    if(0 == pc_ict_Ping())
    {
        if(0==count)
        {
            emit ict_Status(tr("ICT:已连接"));
            emit ictIsReady(true);
            count++;
        }
        QString testResult = "";
        QString openSignal = "";
        QString errorMsg = "";
        getIctInfo(ICT_LOCAL_RESULT_NAME,testResult);
        getIctInfo(ICT_LOCAL_OPEN_NAME,openSignal);
        getIctInfo(ICT_LOCAL_ERROR_NAME,errorMsg);
        if(!testResult.isEmpty())
        {
            result = testResult;
        }
        if(!openSignal.isEmpty())
        {
            if("2"==openSignal)
                canOpen = true;
        }

        updateTestResult();

        if(!errorMsg.isEmpty())
        {
            emit ict_Error_Msg(QString("ICT error:%1\n").arg(errorMsg));//错误信息还未定义，可能需要需要转换解码
        }
        return;
    }
    emit ict_Status(tr("ICT:已断开"));
    emit ictIsReady(false);
    count = 0;

    if(statusReadTimer->isActive())
        statusReadTimer->stop();

    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}

void ICT_Test_Obj::openTimer()
{
    if(!statusReadTimer->isActive())
        statusReadTimer->start(500);
}

void ICT_Test_Obj::updateTestResult()
{
    if(!result.isEmpty() && true == canOpen)
    {
        emit ictTestResult(result);
        result = "";
        canOpen = false;
    }
}

void ICT_Test_Obj::testStart(QString sn)
{
    if(0 == pc_ict_Ping())
    {
        setIctInfo(ICT_LOCAL_SN_NAME,sn);
        setIctInfo(ICT_LOCAL_CLOSE_NAME,"3");
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}
