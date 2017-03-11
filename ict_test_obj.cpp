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
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        emit ict_Error_Msg(tr("打开ICT测试机的本地文件失败!\n"));
        return ;
    }
    QTextStream in_out(&file);
    readMsg = in_out.readAll();
    if(!readMsg.isEmpty())
    {//刷新文件，然后清空文件
        file.flush();
        in_out << "";
    }
    file.close();
    return;
}

void ICT_Test_Obj::setIctInfo(QString fileName, QString writeMsg)
{
    QFile file(QString(ICT_LOCAL_FILE_PATH).arg(fileName));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit ict_Error_Msg(tr("打开ICT测试机的本地文件失败!\n"));
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
    statusReadTimer = new QTimer(this);
    connect(statusReadTimer,&QTimer::timeout,this,&ICT_Test_Obj::statusReadTimeout);
    statusReadTimer->start(500);
}

void ICT_Test_Obj::statusReadTimeout()
{
    if(0 == pc_ict_Ping())
    {
        QString testResult;
        QString openSignal;
        QString errorMsg;
        getIctInfo(ICT_LOCAL_RESULT_NAME,testResult);
        getIctInfo(ICT_LOCAL_OPEN_NAME,openSignal);
        getIctInfo(ICT_LOCAL_ERROR_NAME,errorMsg);
        if(!testResult.isEmpty())
        {
            emit ictTestResult(testResult);
        }
        if(!openSignal.isEmpty())
        {
            emit ictCanOpen();
        }
        if(!errorMsg.isEmpty())
        {
            emit ict_Error_Msg(errorMsg+"\n");//错误信息还未定义，可能需要需要转换解码
        }
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败！\n"
                          "请检查本机与ICT的网线连接以及IP地址是否正确！\n"));
    return ;
}
