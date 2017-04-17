#include "ict_test_obj.h"
#include "staticname.h"
#include "language.h"
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QProcess>
#include <QDateTime>
#include <QSettings>
#include <QRegExp>
#include <QThread>

ICT_Test_Obj::ICT_Test_Obj(QObject *parent) : QObject(parent)
{

}

void ICT_Test_Obj::getIctInfo(QString fileName, QString &readMsg)
{
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString ict_drive = configRead->value(ICT_LOCAL_DRIVE).toString();
    delete configRead;
    QString ICT_path = QString("%1:\\%2").arg(ict_drive).arg(fileName);

    QFile file(ICT_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(QString(tr("打开ICT测试机的本地文件(%1)失败!\n")).arg(fileName));
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
    QString ict_drive = configRead->value(ICT_LOCAL_DRIVE).toString();
    delete configRead;
    QString ICT_path = QString("%1:\\%2").arg(ict_drive).arg(fileName);

    QFile file(ICT_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(QString(tr("打开ICT测试机的本地文件(%1)失败!\n")).arg(fileName));
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
    QString receive_file_name = configRead->value(ICT_LOCAL_RECEIVE_FILE_NAME).toString();
    QString receive_name = configRead->value(ICT_LOCAL_RECEIVE_NAME).toString();
    QString ict_drive = configRead->value(ICT_LOCAL_DRIVE).toString();
    delete configRead;
    QString receive_path = QString("%1/%2").arg(receive_file_name).arg(receive_name);
    QString ICT_path = QString("%1:\\%2").arg(ict_drive).arg(receive_path);

    QFile file(ICT_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(QString(tr("连接ICT测试机的本地文件失败!\n")));
        ictStatusStr = tr("ICT:已断开");
        emit ict_Status(QString("%1,%2").arg(ictStatusStr).arg(ictEnableStr));
        emit ictIsReady(false);
        file.close();
        count = 0;
        return -1;
    }
    file.close();
    if(0==count)
    {
        ictStatusStr = tr("ICT:已连接");
        emit ict_Status(QString("%1,%2").arg(ictStatusStr).arg(ictEnableStr));
        emit ictIsReady(true);
        count++;
    }
    return 0;
}

void ICT_Test_Obj::init_ict()
{
    count = 0;
    ictEnable = true;
    ictEnableStr = tr("已启用");
    statusReadTimer = new QTimer(this);
    testTimer = new QTimer(this);
    snTemp = "";
    snCheckCount = -1;
    testResultTemp = "";
    testRunning = false;
    hold_on_Timer = new QTimer(this);

    connect(statusReadTimer,&QTimer::timeout,this,&ICT_Test_Obj::statusReadTimeout);
    connect(testTimer,&QTimer::timeout,this,&ICT_Test_Obj::testTimeout);
    connect(hold_on_Timer,&QTimer::timeout,this,&ICT_Test_Obj::hold_on_Timeout);
    openTimer();
    pc_ict_Ping();
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

        /*获取SN check反馈*/
        if(!snTemp.isEmpty()&&0==snCheckCount)
        {
            QString sn_receiveStr = "";
            getIctInfo(receive_path, sn_receiveStr);
            if(!sn_receiveStr.isEmpty())
            {
                emit forShow_To_Comm(forShowReceiveString(sn_receiveStr));
                if(sn_receiveStr.contains(snTemp)/* || true*/)
                {
                    if(sn_receiveStr.contains("PASS"))
                    {
                        emit ict_Check_SN_Result(snTemp,true);
                    }
                    else
                    {
                        if(sn_receiveStr.contains("FAIL"))
                        {
                            emit ict_Check_SN_Result(snTemp,false);
                        }
                        else
                        {
                            emit ict_Error_Msg(QString(tr("ICT回传SN Check结果格式异常：\n%1\n")).arg(sn_receiveStr));
                        }
                    }
                    snCheckCount = 1;
                }
                else
                {
                    emit ict_Error_Msg(QString(tr("Scan条码与ICT回传条码不一致：\n"
                                          "Scan：%1; ICT：%2\n")).arg(snTemp).arg(sn_receiveStr));
                }
            }
        }
        /*获取test result反馈*/
        if(true==testRunning&&!snTemp.isEmpty()&&1==snCheckCount)
        {
            QString testResultStr = "";
            getIctInfo(result_path, testResultStr);
            if(!testResultStr.isEmpty())
            {
                emit forShow_To_Comm(forShowReceiveString(testResultStr));
                /*"N08,B,ICT,J1114586,2I0801, ,ICT_V13,SN1234567890,P, , ,0,"*/
                /*"C4-L1N,A,ICT,J1741705,3I1101, ,ICT_V04R03,AH12016602,P, , ,0,"*/
                QRegExp resultRE("(.*),(.*),(.*),(.*),(.*),(.*),(.*),(.*),(.*),(.*),(.*),(.*),(.*)");
                if(0 <= testResultStr.indexOf(resultRE))
                {
                    if(resultRE.cap(8)==snTemp)
                    {
                        if("P"==resultRE.cap(9))
                        {
                            testResultTemp = "PASS";
                        }
                        else
                        {
                            if("F"==resultRE.cap(9))
                            {
                                testResultTemp = QString("FAIL,%1").arg(resultRE.cap(10));
                            }
                        }
                        send_ictTestResult();
                    }
                    else
                    {
                        emit ict_Error_Msg(QString(tr("Scan条码与ICT测试结果回传条码不一致：\n"
                                              "Scan：%1; ICT：%2\n")).arg(snTemp).arg(resultRE.cap(8)));
                    }
                    snCheckCount = 2;
                }
                else
                {
                    emit ict_Error_Msg(QString(tr("ICT回传测试结果格式异常：\n%1\n")).arg(testResultStr));
                }
            }
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

void ICT_Test_Obj::testStart()//ict开始测试
{
    if(false == ictEnable)
    {
        send_ictTestResult();
        return;
    }
    if(0 == pc_ict_Ping())
    {
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString run_file_name = configRead->value(ICT_LOCAL_RUN_FILE_NAME).toString();
        QString run_name = configRead->value(ICT_LOCAL_RUN_NAME).toString();
        QString result_file_name = configRead->value(ICT_LOCAL_RESULT_FILE_NAME).toString();
        QString result_name = configRead->value(ICT_LOCAL_RESULT_NAME).toString();
        int test_timeout = configRead->value(ICT_TEST_TIMEOUT).toString().toInt();
        delete configRead;

        QString result_path = QString("%1/%2").arg(result_file_name).arg(result_name);
        QString run_path = QString("%1/%2").arg(run_file_name).arg(run_name);
        //启动ICT测试
        setIctInfo(result_path,"");
        setIctInfo(run_path,"RUN");
        testRunning = true;
//        emit openSwitch(CONTROL_OUT2_ON);
        emit forShow_To_Comm(forShowSendString("RUN"));
        if(!testTimer->isActive())
            testTimer->start(test_timeout*1000);//测试超时判断
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
        QThread::msleep(2000);
        emit ict_Check_SN_Result(snTemp,true);
        snTemp = "";
        return;
    }
    if(0 == pc_ict_Ping())
    {
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString sn_file_name = configRead->value(ICT_LOCAL_SN_FILE_NAME).toString();
        QString sn_name = configRead->value(ICT_LOCAL_SN_NAME).toString();
        QString receive_file_name = configRead->value(ICT_LOCAL_RECEIVE_FILE_NAME).toString();
        QString receive_name = configRead->value(ICT_LOCAL_RECEIVE_NAME).toString();
        delete configRead;
        QString receive_path = QString("%1/%2").arg(receive_file_name).arg(receive_name);
        QString sn_path = QString("%1/%2").arg(sn_file_name).arg(sn_name);

        setIctInfo(receive_path,"");
        setIctInfo(sn_path,sn);
        snCheckCount = 0;
        testRunning = false;
        emit openSwitch(CONTROL_OUT2_ON);
        emit forShow_To_Comm(forShowSendString(sn));
        openTimer();
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}

void ICT_Test_Obj::testTimeout()
{
    if(testTimer->isActive())
        testTimer->stop();
    emit ict_light_Red_Green_Yellow_Buzzer("Red light open");
    emit ict_setRunStatus(false);
    emit ict_Error_Msg(tr("ICT测试机测试超时！\nRobot将会复位，请注意安全！\n"));
    emit ict_testTimeout();
}

void ICT_Test_Obj::catchFail()//Robot抓取失败
{
    //return;//用于调试-----------------------------------
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
        //启动ICT测试
        setIctInfo(run_path,"CATCHFAIL");
        emit forShow_To_Comm(forShowSendString("CATCHFAIL"));
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}


QString ICT_Test_Obj::forShowSendString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Send_to_ICT:" + str + "\n";
    return str;
}

QString ICT_Test_Obj::forShowReceiveString(QString str)
{
    str.replace("\r","");
    str.replace("\n","");
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + "Receive_from_ICT:" + str + "\n";
    return str;
}

void ICT_Test_Obj::send_ictTestResult()
{
    if(false == ictEnable)
    {
        testResultTemp = "PASS";
    }

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString vacuum_time = configRead->value(ICT_OPEN_WAIT_TIME).toString();
    delete configRead;
    if(0>=vacuum_time.toInt()||""==vacuum_time)
    {
        vacuum_time = "15";
    }
    if(!hold_on_Timer->isActive())
    {
        hold_on_Timer->start(vacuum_time.toInt()*1000);
    }
}

void ICT_Test_Obj::hold_on_Timeout()
{
    if(hold_on_Timer->isActive())
        hold_on_Timer->stop();
    if("PASS"==testResultTemp || testResultTemp.contains("FAIL"))
    {
        emit ictTestResult(testResultTemp);
        testResultTemp = "";
        snTemp = "";
        snCheckCount = -1;
        testRunning = false;
        if(testTimer->isActive())
            testTimer->stop();
    }
}
