#include "ict_test_obj.h"
#include "staticname.h"
#include "language.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QProcess>
#include <QDateTime>
#include <QSettings>
#include <QDateTime>
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
    ICT_path = QString("..\\test/%2").arg(fileName);//调试用----------

    QFile file(ICT_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(QString(tr("打开ICT测试机的本地文件<%1>失败!\n")).arg(fileName));
        if(statusReadTimer->isActive())
            statusReadTimer->stop();
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
    ICT_path = QString("..\\test/%2").arg(fileName);//调试用----------

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
    int time_s1 = QDateTime::currentDateTime().toString("ss").toInt();

    qDebug()<<"time_s1 return:"<<time_s1;//for debug-----------------------------

    int ref = QProcess::execute(pingStr);

    qDebug()<<"ref return:"<<ref;//for debug--------------------------------------

    int time_s2 = QDateTime::currentDateTime().toString("ss").toInt();

    qDebug()<<"time_s2 return:"<<time_s2;//for debug-------------------------------

    int time_offset = time_s2 - time_s1;

    qDebug()<<"time_offset return:"<<time_offset;//for debug----------------------
    return 0;//调试用--------------------------------

    if(0 == ref && 0 == time_offset)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

void ICT_Test_Obj::init_ict()
{
    count = 0;
    ictEnable = true;
    ictEnableStr = tr("已启用");
    statusReadTimer = new QTimer(this);
    testTimer = new QTimer(this);
    connect(statusReadTimer,&QTimer::timeout,this,&ICT_Test_Obj::statusReadTimeout);
    connect(testTimer,&QTimer::timeout,this,&ICT_Test_Obj::testTimeout);
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
        QString receive_file_name = configRead->value(ICT_LOCAL_RECEIVE_FILE_NAME).toString();
        QString receive_name = configRead->value(ICT_LOCAL_RECEIVE_NAME).toString();
        QString result_file_name = configRead->value(ICT_LOCAL_RESULT_FILE_NAME).toString();
        QString result_name = configRead->value(ICT_LOCAL_RESULT_NAME).toString();
        delete configRead;

        QString receive_path = QString("%1/%2").arg(receive_file_name).arg(receive_name);
        QString result_path = QString("%1/%2").arg(result_file_name).arg(result_name);

        QString receiveStr = "";
        QString testResultStr = "";

        getIctInfo(receive_path, receiveStr);
        if(!receiveStr.isEmpty())
        {
//            if(receiveStr.contains(snTemp))
            if(true)
            {
                if(receiveStr.contains("P"))
                {
                    emit ict_Check_SN_Result(snTemp,true);
                }
                else
                {
                    if(receiveStr.contains("F"))
                    {
                        emit ict_Check_SN_Result(snTemp,false);
                    }
                }
            }
            else
            {
                emit ict_Error_Msg(QString(tr("Scan条码与ICT回传条码不一致：\n"
                                      "Scan：%1; ICT：%2\n")).arg(snTemp).arg(receiveStr));
            }
        }
        getIctInfo(result_path, testResultStr);
        if(!testResultStr.isEmpty())
        {
            int index = testResultStr.indexOf(snTemp);
            index = index + snTemp.length() + 1;
            QString strTemp = testResultStr.mid(index,1);
            if("P"==strTemp)
            {
                emit ictTestResult("PASS");
            }
            else
            {
                if("F"==strTemp)
                {
                    emit ictTestResult("FAIL");
                }
            }
            snTemp = "";
            if(!testTimer->isActive())
                testTimer->stop();//测试超时判断
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

void ICT_Test_Obj::openTimer()//备用函数，用于后期重新打开定时器
{
    if(!statusReadTimer->isActive())
        statusReadTimer->start(500);
}

void ICT_Test_Obj::testStart()//ict开始测试
{
    if(false == ictEnable)
    {
        return;
    }
    if(0 == pc_ict_Ping())
    {
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString run_file_name = configRead->value(ICT_LOCAL_RUN_FILE_NAME).toString();
        QString run_name = configRead->value(ICT_LOCAL_RUN_NAME).toString();
        delete configRead;
        QString run_path = QString("%1/%2").arg(run_file_name).arg(run_name);
        setIctInfo(run_path,"RUN");
        if(!testTimer->isActive())
            testTimer->start(3*60*1000);//测试超时判断
        emit openSwitch(CONTROL_OUT2_ON);
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

void ICT_Test_Obj::ict_Check_SN(QString sn)//将SN传递给ICT作SN Check
{
    snTemp = sn;
    snTemp.replace("\r","");
    snTemp.replace("\n","");
    if(false == ictEnable)
    {
        return;
    }
    if(0 == pc_ict_Ping())
    {
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString sn_file_name = configRead->value(ICT_LOCAL_SN_FILE_NAME).toString();
        QString sn_name = configRead->value(ICT_LOCAL_SN_NAME).toString();
        delete configRead;

        QString sn_path = QString("%1/%2").arg(sn_file_name).arg(sn_name);

        setIctInfo(sn_path,sn);
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}

void ICT_Test_Obj::testTimeout()
{
    emit ict_light_Red_Green_Yellow_Buzzer("Red light open");
    emit ict_Error_Msg(tr("ICT测试机测试超时！\n"));
}
