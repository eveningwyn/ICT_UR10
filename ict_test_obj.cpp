#include "ict_test_obj.h"
#include "staticname.h"
#include "language.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QProcess>
#include <QDateTime>
#include <QSettings>
#include <QDebug>

ICT_Test_Obj::ICT_Test_Obj(QObject *parent) : QObject(parent)
{

}

void ICT_Test_Obj::getIctInfo(QString fileName, QString &readMsg)
{
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString ict_ip_addr = configRead->value(ICT_LOCAL_IP).toString();
    delete configRead;
    QString ICT_path = QString("//%1/%2").arg(ict_ip_addr).arg(fileName);

    QFile file(ICT_path);
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
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString ict_ip_addr = configRead->value(ICT_LOCAL_IP).toString();
    delete configRead;
    QString ICT_path = QString("//%1/%2").arg(ict_ip_addr).arg(fileName);

    QFile file(ICT_path);
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
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString ict_ip_addr = configRead->value(ICT_LOCAL_IP).toString();
    delete configRead;
    QString pingStr = "ping " + ict_ip_addr + " -n 1";
    int ref = QProcess::execute(pingStr);
    return ref;
}

void ICT_Test_Obj::init_ict()
{
    count = 0;
    result = "";
    canOpen = false;
    ictEnable = true;
    ictEnableStr = tr("已启用");
    statusReadTimer = new QTimer(this);
    connect(statusReadTimer,&QTimer::timeout,this,&ICT_Test_Obj::statusReadTimeout);
    statusReadTimer->start(500);
}

void ICT_Test_Obj::statusReadTimeout()
{
    if(false == ictEnable)
    {
        return;
    }
    if(0 == pc_ict_Ping())
    {
        if(0==count)
        {
            ictStatusStr = tr("ICT:已连接");
            emit ict_Status(QString("%1,%2").arg(ictStatusStr).arg(ictEnableStr));
            emit ictIsReady(true);
            count++;
        }
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString result_file_name = configRead->value(ICT_LOCAL_RESULT_FILE_NAME).toString();
        QString result_name = configRead->value(ICT_LOCAL_RESULT_NAME).toString();
        QString open_file_name = configRead->value(ICT_LOCAL_OPEN_FILE_NAME).toString();
        QString open_name = configRead->value(ICT_LOCAL_OPEN_NAME).toString();
        QString error_file_name = configRead->value(ICT_LOCAL_ERROR_FILE_NAME).toString();
        QString error_name = configRead->value(ICT_LOCAL_ERROR_NAME).toString();
        delete configRead;

        QString result_path = QString("%1/%2").arg(result_file_name).arg(result_name);
        QString open_path = QString("%1/%2").arg(open_file_name).arg(open_name);
        QString error_path = QString("%1/%2").arg(error_file_name).arg(error_name);

        QString testResult = "";
        QString openSignal = "";
        QString errorMsg = "";
        getIctInfo(result_path, testResult);
        getIctInfo(open_path, openSignal);
        getIctInfo(error_path, errorMsg);
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

    ictStatusStr = tr("ICT:已断开");
    emit ict_Status(QString("%1,%2").arg(ictStatusStr).arg(ictEnableStr));
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
    if(false == ictEnable)
    {
        return;
    }
    if(0 == pc_ict_Ping())
    {
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString sn_file_name = configRead->value(ICT_LOCAL_SN_FILE_NAME).toString();
        QString sn_name = configRead->value(ICT_LOCAL_SN_NAME).toString();
        QString close_file_name = configRead->value(ICT_LOCAL_CLOSE_FILE_NAME).toString();
        QString close_name = configRead->value(ICT_LOCAL_CLOSE_NAME).toString();
        delete configRead;

        QString sn_path = QString("%1/%2").arg(sn_file_name).arg(sn_name);
        QString close_path = QString("%1/%2").arg(close_file_name).arg(close_name);

        setIctInfo(sn_path,sn);
        setIctInfo(close_path,"3");
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}

void ICT_Test_Obj::set_ictEnable(bool enable)
{
    ictEnable = enable;
    if(true == ictEnable)
    {
        ictEnableStr = tr("已启用");
    }
    else
    {
        ictEnableStr = tr("未启用");
    }
    emit ict_Status(QString("%1,%2").arg(ictStatusStr).arg(ictEnableStr));
}
