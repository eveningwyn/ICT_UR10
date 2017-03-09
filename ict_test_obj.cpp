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

void ICT_Test_Obj::getIctInfo(QString path)
{
    if(0 == ictPing())
    {
        QFile file(QString(ICT_LOCAL_TEST_PATH).arg(path));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit ict_Error_Msg(tr("打开ICT测试机的本地文件失败!\n"));
            return ;
        }
        QTextStream in(&file);
        QString strTemp;
        strTemp = in.readAll();
        file.close();
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败!\n"));
    return ;
}

void ICT_Test_Obj::setIctInfo(QString path)
{
    if(0 == ictPing())
    {
        QString strTemp = "START\n";//调试用

        QFile file(QString(ICT_LOCAL_TEST_PATH).arg(path));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            emit ict_Error_Msg(tr("打开ICT测试机的本地文件失败!\n"));
            return ;
        }
        QTextStream out(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);//鼠标指针变为等待状态
        out << strTemp;
        QApplication::restoreOverrideCursor();//鼠标指针恢复原来的状态
        file.close();
        return;
    }
    emit ict_Error_Msg(tr("与ICT测试机的网络PING失败!\n"));
    return ;
}

int ICT_Test_Obj::ictPing()
{
    QString ip = ICT_LOCAL_IP;
    QString pingStr = "ping " + ip + " -n 1";
    int ref = QProcess::execute(pingStr);
    return ref;
}
