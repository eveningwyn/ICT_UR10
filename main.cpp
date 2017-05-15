#include "ict_ur10.h"
#include <QApplication>
//  #include <stdio.h>
//  #include <stdlib.h>
#include <QException>
#include <Windows.h>
#include <DbgHelp.h>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QPixmap>
#include <QSplashScreen>

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
        break;
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("..\\log/crash-log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    {
        QTextStream text_stream(&file);
        text_stream << message << "\r\n";
        file.flush();
        file.close();
    }
    mutex.unlock();
}

long ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    //这里弹出一个错误对话框并退出程序
//    EXCEPTION_RECORD* record = pException->ExceptionRecord;
//    QString errCode(QString::number(record->ExceptionCode,16));
//    QString errAdr(QString::number((uint)record->ExceptionAddress,16));
    //QString errMod;
//    qCritical("this is error!");
//    QMessageBox::critical(NULL,"Crash",QString("Error Code: %1\nError Addr: %2\n")
//                          .arg(errCode).arg(errAdr),QMessageBox::Ok);



    //Create the dump file
//    HANDLE hDumpFile = CreateFile((LPCWSTR)QString("..//log/crash.dmp").utf16(),
//                                  GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hDumpFile = CreateFile((LPCWSTR)QString("..//log/crash.dmp").utf16(),
                                  GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDumpFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
//        dumpInfo.ClientPointers = TRUE;
        dumpInfo.ClientPointers = FALSE;
//        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpWithFullMemory, &dumpInfo, NULL, NULL);
        CloseHandle(hDumpFile);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/res/Industrial_robot_128px_532261_easyicon.net.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    //注册MessageHandler
    qInstallMessageHandler(outputMessage);
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数

    ICT_UR10 w;
    w.show();

    splash.finish(&w);

    return a.exec();
}
