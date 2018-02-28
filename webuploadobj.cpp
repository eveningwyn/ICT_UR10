#include "webuploadobj.h"
#include "language.h"
#include <QUuid>
#include <QtXml>
#include <QSettings>
#include <QString>
#include "staticname.h"
#include <QDebug>

//#define WEB_SERVEICE         "http://ezconsf.joytech.com.cn/WSCenter/WSCenterEntrance.svc"
#define WEB_SERVEICE         "http://10.10.30.96/WSCenter/WSCenterEntrance.svc"
#define WEB_SERVEICE_TEST    "http://10.2.24.153/WSCenter/WSCenterEntrance.svc"
#define WEB_SERVEICE_IP      "210.75.11.119"

#define GET_CONFIG           "GetConfigSetting"
#define GET_SFDATA           "getSFData"
#define CHECKIN              "CheckIn"
#define TESTRESULT           "TestResult"

WebUploadObj::WebUploadObj(QObject *parent) : QObject(parent)
{
    init_web();
}

WebUploadObj::~WebUploadObj()
{
}

void WebUploadObj::init_web()
{
    V_UID = QUuid::createUuid().toString();//取得唯一的GUID码
    qDebug()<<"V_UID: "<<V_UID;
    V_UID.replace("{","");
    V_UID.replace("}","");
    qDebug()<<"V_UID"<<V_UID;
    web_manager = new QNetworkAccessManager(this);
    connect(web_manager,&QNetworkAccessManager::finished,this,&WebUploadObj::replyFinished);
}

void WebUploadObj::replyFinished(QNetworkReply *reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug()<<"status_code: "<<status_code;

    if(QNetworkReply::NoError == reply->error())
    {
        QByteArray replyBytes = reply->readAll();

        QString replyStr(replyBytes);//转化为字符串
        qDebug()<<QString(tr("web应答信息:\n%1\n")).arg(replyStr);
        emit web_Error_Msg(QString(tr("web应答信息:\n%1\n")).arg(replyStr));

        QDomDocument doc;//创建Dom类对象解析xml
        if(doc.setContent(replyStr))
        {
            QDomNode node = doc.documentElement().elementsByTagName("ROWSET").at(0);
            node = node.toElement().elementsByTagName("OUTPUT").at(0);
            replyStr = node.toElement().text();//得到xml中某个节点的值
        }
        qDebug()<<QString(tr("信息处理:\n%1\n")).arg(replyStr);
        emit web_Error_Msg(QString(tr("信息处理:\n%1\n")).arg(replyStr));
    }
    else
    {
        //处理错误
        emit web_Error_Msg(QString(tr("web应答信息异常!\n%1\n")).arg(reply->errorString()));

        qDebug()<<reply->readAll();
    }
    reply->deleteLater();
}

void WebUploadObj::callWebFunc(QString intputMsg)
{
    //DUNS_CODE:连接资料库主机("SF-TEST"/""/"ERP-0-10");
    //ZIP:是否有压缩;
    //UID:当次系统追踪ID;
    //USER_ID:员工编号("A1234567"/"SF"/"J1745259");
    //IS_ONE_WAY:不接收结果;
    //OUTPUT_FORMAT:输出资料格式(XML);
    //SP_NAME:(留空);
    //ACTION_CODE:服务(ICT_ManufacturerClassLibrary);
    //LIBRARY_ACTION_CODE:方法Function;
    //ROWSET/INPUT:INPUT参数;
    QString intputString = "";

    intputString = QString("<INPUTSTRING>"
                           "<DUNS_CODE>%1</DUNS_CODE>"
                           "<ZIP>FALSE</ZIP>"
                           "<UID>%2</UID>"
                           "<USER_ID>%3</USER_ID>"
                           "<IS_ONE_WAY>FALSE</IS_ONE_WAY>"
                           "<OUTPUT_FORMAT>XML</OUTPUT_FORMAT>"
                           "<SP_NAME></SP_NAME>"
                           "<ACTION_CODE>%4</ACTION_CODE>"
                           "<LIBRARY_ACTION_CODE>%5</LIBRARY_ACTION_CODE>"
                           "<ROWSET><INPUT>%6</INPUT></ROWSET>"
                           "</INPUTSTRING>")
            .arg("ERP-0-10").arg(V_UID).arg("J1745259")
            .arg("ICT_ManufacturerClassLibrary").arg("ICT_Board_Select_Add").arg(intputMsg);

//    intputString = QString("<INPUTSTRING>"
//                           "<ACTION_CODE>ICT_RobotClassLibrary</ACTION_CODE>"
//                           "<LIBRARY_ACTION_CODE>%1+</LIBRARY_ACTION_CODE>"
//                           "<ROWSET><INPUT>%2</INPUT></ROWSET>"
//                           "</INPUTSTRING>")
//            .arg("ICT_Board_Select_Center").arg(intputMsg);

    postWeb(intputString);
}

void WebUploadObj::postWeb(const QString postMsg)
{
//    qDebug()<<"postMsg--"<<postMsg;
//    QUrl url(QString(WEB_SERVEICE));
    QUrl url(QString("http://localhost:20376/WebService1.asmx"));//http://localhost:20376/WebService1.asmx; http://localhost:20376/WebService1.asmx?wsdl
    QByteArray bytePostData = "";
    bytePostData = postMsg.toLocal8Bit();
//    qDebug()<<"bytePostData--"<<bytePostData;
    web_request.setUrl(url);
    web_request.setHeader(QNetworkRequest::ContentTypeHeader,"text/xml;charset=utf-8");
    web_request.setHeader(QNetworkRequest::ContentLengthHeader,bytePostData.length());
    web_request.setRawHeader("Content-Type","text/xml;charset=utf-8");
    web_manager->post(web_request,bytePostData);
}
void WebUploadObj::msgUpload(const QString state, const QString startTime, const QString endTime, const QString errorCode)
{
    QString strPostMsg = QString("<DATA>"
                                 "<LINEALSNAME>%1</LINEALSNAME>"
                                 "<ARMNAME>%2</ARMNAME>"
                                 "<STATUS>%3</STATUS>"
                                 "<STARTTIME>%4</STARTTIME>"
                                 "<ENDTIME>%5</ENDTIME>"
                                 "<ERRORCODE>%6</ERRORCODE>"
                                 "</DATA>")
            .arg("自动化1").arg("1#").arg(state).arg(startTime).arg(endTime).arg(errorCode);
//    postWebNew(strPostMsg);
    callWebFunc(strPostMsg);
}

typedef void (*DllFunc)(std::string INPUTSTRING);
void WebUploadObj::postWebNew(const QString postMsg)
{
    QString sDllFileName;
//    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
//    sDllFileName = configRead->value(ICT_LOCAL_DRIVE).toString();
//    delete configRead;
    sDllFileName = "http://10.10.30.96/WSCenter/WSCenterEntrance.svc/ICT_ManufacturerClassLibrary";//for debug

    QLibrary webLib(sDllFileName);
    if(webLib.load())
    {
        DllFunc ICT_Board_Select_Add = (DllFunc)webLib.resolve("ICT_Board_Select_Add");
        if(ICT_Board_Select_Add)
        {
            ICT_Board_Select_Add(postMsg.toStdString());
        }
        else {
            qDebug()<<QString(tr("服务器DLL函数调用失败, 路径: %1.\n").arg(sDllFileName));
            emit web_Error_Msg(QString(tr("服务器DLL函数调用失败, 路径: %1.\n").arg(sDllFileName)));
        }
    }
    else {
        qDebug()<<QString(tr("服务器DLL调用失败, 路径: %1.\n").arg(sDllFileName));
        emit web_Error_Msg(QString(tr("服务器DLL调用失败, 路径: %1.\n").arg(sDllFileName)));
    }
}
void WebUploadObj::forTest()//for debug
{
    QString sDllFileName;
    //sDllFileName = "http://10.10.30.96/WSCenter/WSCenterEntrance.svc/ICT_ManufacturerClassLibrary";
    sDllFileName = "http://localhost:20376/WebService1.asmx?op=ICT_Board_Select_add";

    QLibrary webLib(sDllFileName);
    if(webLib.load())
    {
        qDebug()<<QString(tr("服务器DLL调用成功, 路径: %1.\n").arg(sDllFileName));
        emit web_Error_Msg(QString(tr("服务器DLL调用成功, 路径: %1.\n").arg(sDllFileName)));
        DllFunc ICT_Board_Select_Add = (DllFunc)webLib.resolve("ICT_Board_Select_Add");
        if(ICT_Board_Select_Add)
        {
            ICT_Board_Select_Add("");
            qDebug()<<QString(tr("服务器DLL函数调用成功, 路径: %1.\n").arg(sDllFileName));
            emit web_Error_Msg(QString(tr("服务器DLL函数调用成功, 路径: %1.\n").arg(sDllFileName)));
        }
        else {
            qDebug()<<QString(tr("服务器DLL函数调用失败, 路径: %1.\n").arg(sDllFileName));
            emit web_Error_Msg(QString(tr("服务器DLL函数调用失败, 路径: %1.\n").arg(sDllFileName)));
        }
    }
    else {
        qDebug()<<QString(tr("服务器DLL调用失败, 路径: %1.\n").arg(sDllFileName));
        emit web_Error_Msg(QString(tr("服务器DLL调用失败, 路径: %1.\n").arg(sDllFileName)));
    }
}
