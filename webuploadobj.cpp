#include "webuploadobj.h"
#include "language.h"
#include <QUuid>
#include <QtXml>
#include <QSettings>
#include <QString>
#include "staticname.h"
#include <QDebug>
#include "WCF_WC/soapBasicHttpBinding_USCOREIWSCenterEntranceProxy.h"
#include "WCF_WC/soapH.h"
#include "WCF_WC/soapStub.h"
#include "WCF_WC/stdsoap2.h"
#include "WCF_WC/BasicHttpBinding_USCOREIWSCenterEntrance.nsmap"


#define WEB_SERVEICE         "http://10.10.30.96/WSCenter/WSCenterEntrance.svc"
#define WEB_SERVEICE_TEST    "http://210.75.11.119/WSCenter/WSCenterEntrance.svc"
#define WEB_SERVEICE_TEST_NULL    NULL

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
    V_UID.replace("{","");
    V_UID.replace("}","");
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

void WebUploadObj::postWeb(const QString postMsg)//Old Code
{
//    qDebug()<<"postMsg--"<<postMsg;
//    QUrl url(QString(WEB_SERVEICE));
    //http://localhost:20376/WebService1.asmx; http://localhost:20376/WebService1.asmx?wsdl
    QUrl url(QString("http://10.10.30.96/WSCenter/WSCenterEntrance.svc?wsdl"));
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
    QString xmlStr = getXmlString(state,startTime,endTime,errorCode);
    callWebService(xmlStr);
    callWebServiceTest(xmlStr);
    return;

    QString inputStr = "";
    inputStr = QString("<DATA>"
                       "<LINEALSNAME>%1</LINEALSNAME>"
                       "<ARMNAME>%2</ARMNAME>"
                       "<STATUS>%3</STATUS>"
                       "<STARTTIME>%4</STARTTIME>"
                       "<ENDTIME>%5</ENDTIME>"
                       "<ERRORCODE>%6</ERRORCODE>"
                       "</DATA>")
            .arg("自动化1").arg("1#")
            .arg(state).arg(startTime).arg(endTime).arg(errorCode);

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
    QString strPostMsg = "";
    strPostMsg = QString("<INPUTSTRING>"
                         "<DUNS_CODE>%1</DUNS_CODE>"
                         "<ZIP>%2</ZIP>"
                         "<UID>%3</UID>"
                         "<USER_ID>%4</USER_ID>"
                         "<IS_ONE_WAY>%5</IS_ONE_WAY>"
                         "<OUTPUT_FORMAT>%6</OUTPUT_FORMAT>"
                         "<SP_NAME>%7</SP_NAME>"
                         "<ACTION_CODE>%8</ACTION_CODE>"
                         "<LIBRARY_ACTION_CODE>%9</LIBRARY_ACTION_CODE>"
                         "<ROWSET><INPUT>%10</INPUT></ROWSET>"
                         "</INPUTSTRING>")
            .arg("ERP-0-10").arg("FALSE").arg(V_UID)
            .arg("J1745259").arg("FALSE").arg("XML")
            .arg("").arg("ICT_ManufacturerClassLibrary").arg("ICT_Board_Select_Add")
            .arg(inputStr);

    callWebService(strPostMsg);
    callWebServiceTest(strPostMsg);//for test
}

QString WebUploadObj::getXmlString(const QString state, const QString startTime, const QString endTime, const QString errorCode)
{
    QDomDocument doc;
//    //写入xml头部
//    QDomProcessingInstruction instruction; //添加处理命令
//    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
//    doc.appendChild(instruction);

//添加 DATA 根节点
    QDomElement rootDATA = doc.createElement("DATA");
    doc.appendChild(rootDATA);
    //添加 LINEALSNAME 节点
    QDomElement e_LINEALSNAME = doc.createElement("LINEALSNAME");
    QDomText t_LINEALSNAME;
    t_LINEALSNAME = doc.createTextNode("自动化1");//自动化1
    e_LINEALSNAME.appendChild(t_LINEALSNAME);
    rootDATA.appendChild(e_LINEALSNAME);
    //添加 ARMNAME 节点
    QDomElement e_ARMNAME = doc.createElement("ARMNAME");
    QDomText t_ARMNAME;
    t_ARMNAME = doc.createTextNode("1#");
    e_ARMNAME.appendChild(t_ARMNAME);
    rootDATA.appendChild(e_ARMNAME);
    //添加 STATUS 节点
    QDomElement e_STATUS = doc.createElement("STATUS");
    QDomText t_STATUS;
    t_STATUS = doc.createTextNode(state);
    e_STATUS.appendChild(t_STATUS);
    rootDATA.appendChild(e_STATUS);
    //添加 STARTTIME 节点
    QDomElement e_STARTTIME = doc.createElement("STARTTIME");
    QDomText t_STARTTIME;
    t_STARTTIME = doc.createTextNode(startTime);
    e_STARTTIME.appendChild(t_STARTTIME);
    rootDATA.appendChild(e_STARTTIME);
    //添加 ENDTIME 节点
    QDomElement e_ENDTIME = doc.createElement("ENDTIME");
    QDomText t_ENDTIME;
    t_ENDTIME = doc.createTextNode(endTime);
    e_ENDTIME.appendChild(t_ENDTIME);
    rootDATA.appendChild(e_ENDTIME);
    //添加 ERRORCODE 节点
    QDomElement e_ERRORCODE = doc.createElement("ERRORCODE");
    QDomText t_ERRORCODE;
    t_ERRORCODE = doc.createTextNode(errorCode);
    e_ERRORCODE.appendChild(t_ERRORCODE);
    rootDATA.appendChild(e_ERRORCODE);

//添加 INPUTSTRING 根节点
    QDomElement rootINPUTSTRING = doc.createElement("INPUTSTRING");
    doc.appendChild(rootINPUTSTRING);
    //添加 DUNS_CODE 节点
    QDomElement e_DUNS_CODE = doc.createElement("DUNS_CODE");
    QDomText t_DUNS_CODE;
    t_DUNS_CODE = doc.createTextNode("ERP-0-10");
    e_DUNS_CODE.appendChild(t_DUNS_CODE);
    rootINPUTSTRING.appendChild(e_DUNS_CODE);
    //添加 ZIP 节点
    QDomElement e_ZIP = doc.createElement("ZIP");
    QDomText t_ZIP;
    t_ZIP = doc.createTextNode("FALSE");
    e_ZIP.appendChild(t_ZIP);
    rootINPUTSTRING.appendChild(e_ZIP);
    //添加 UID 节点
    QDomElement e_UID = doc.createElement("UID");
    QDomText t_UID;
    t_UID = doc.createTextNode(V_UID);
    e_UID.appendChild(t_UID);
    rootINPUTSTRING.appendChild(e_UID);
    //添加 USER_ID 节点
    QDomElement e_USER_ID = doc.createElement("USER_ID");
    QDomText t_USER_ID;
    t_USER_ID = doc.createTextNode("J1745259");
    e_USER_ID.appendChild(t_USER_ID);
    rootINPUTSTRING.appendChild(e_USER_ID);
    //添加 IS_ONE_WAY 节点
    QDomElement e_IS_ONE_WAY = doc.createElement("IS_ONE_WAY");
    QDomText t_IS_ONE_WAY;
    t_IS_ONE_WAY = doc.createTextNode("FALSE");
    e_IS_ONE_WAY.appendChild(t_IS_ONE_WAY);
    rootINPUTSTRING.appendChild(e_IS_ONE_WAY);
    //添加 OUTPUT_FORMAT 节点
    QDomElement e_OUTPUT_FORMAT = doc.createElement("OUTPUT_FORMAT");
    QDomText t_OUTPUT_FORMAT;
    t_OUTPUT_FORMAT = doc.createTextNode("XML");
    e_OUTPUT_FORMAT.appendChild(t_OUTPUT_FORMAT);
    rootINPUTSTRING.appendChild(e_OUTPUT_FORMAT);
    //添加 SP_NAME 节点
    QDomElement e_SP_NAME = doc.createElement("SP_NAME");
    QDomText t_SP_NAME;
    t_SP_NAME = doc.createTextNode("");
    e_SP_NAME.appendChild(t_SP_NAME);
    rootINPUTSTRING.appendChild(e_SP_NAME);
    //添加 ACTION_CODE 节点
    QDomElement e_ACTION_CODE = doc.createElement("ACTION_CODE");
    QDomText t_ACTION_CODE;
    t_ACTION_CODE = doc.createTextNode("ICT_ManufacturerClassLibrary");
    e_ACTION_CODE.appendChild(t_ACTION_CODE);
    rootINPUTSTRING.appendChild(e_ACTION_CODE);
    //添加 LIBRARY_ACTION_CODE 节点
    QDomElement e_LIBRARY_ACTION_CODE = doc.createElement("LIBRARY_ACTION_CODE");
    QDomText t_LIBRARY_ACTION_CODE;
    t_LIBRARY_ACTION_CODE = doc.createTextNode("ICT_Board_Select_Add");
    e_LIBRARY_ACTION_CODE.appendChild(t_LIBRARY_ACTION_CODE);
    rootINPUTSTRING.appendChild(e_LIBRARY_ACTION_CODE);
    //添加 ROWSET 节点
    QDomElement e_ROWSET = doc.createElement("ROWSET");
    //添加 INPUT 节点
    QDomElement e_INPUT = doc.createElement("INPUT");
    e_INPUT.appendChild(rootDATA);
    e_ROWSET.appendChild(e_INPUT);
    rootINPUTSTRING.appendChild(e_ROWSET);

//    qDebug()<< doc.toString();

    //输出到文件
    QFile file("PostWebData.xml"); //相对路径、绝对路径、资源路径都可以
    if(file.open(QFile::WriteOnly|QFile::Truncate))//可以用QIODevice，Truncate表示清空原来的内容
    {
        QTextStream out_stream(&file);
        doc.save(out_stream,4); //缩进4格
        file.close();
    }

    return doc.toString();
}

void WebUploadObj::callWebService(const QString inputStr)
{
    char cInput[1024];
    strcpy(cInput, inputStr.toLocal8Bit().data());

    _ns1__ENTRANCE *m_ns1__ENTRANCE = new _ns1__ENTRANCE();
    m_ns1__ENTRANCE->INPUTSTRING = cInput;
    _ns1__ENTRANCEResponse m_ns1__ENTRANCEResponse;
    BasicHttpBinding_USCOREIWSCenterEntranceProxy m_BasicHttpBinding_USCOREIWSCenterEntranceProxy;
    int iReturn = -100;
    iReturn = m_BasicHttpBinding_USCOREIWSCenterEntranceProxy.ENTRANCE(WEB_SERVEICE_TEST_NULL,NULL,m_ns1__ENTRANCE,m_ns1__ENTRANCEResponse);
    QString sReturn = QString::fromLocal8Bit(m_ns1__ENTRANCEResponse.ENTRANCEResult);
//    qDebug()<<QString(tr("ENTRANCE函数返回值: %1\n"
//                         "ENTRANCE调用返回字符串:\n%2.\n")
//                      .arg(iReturn).arg(sReturn));
    emit web_Error_Msg(QString(tr("ENTRANCE函数返回值: %1\n"
                                  "ENTRANCE调用返回字符串:\n%2.\n")
                               .arg(iReturn).arg(sReturn)));
    delete m_ns1__ENTRANCE;
}

void WebUploadObj::callWebServiceTest(const QString inputStr)
{
    char cInput[1024];
    strcpy(cInput, inputStr.toLocal8Bit().data());

    _ns1__ENTRANCE_USCORETEST *m_ns1__ENTRANCE_USCORETEST = new _ns1__ENTRANCE_USCORETEST();
    m_ns1__ENTRANCE_USCORETEST->INPUTSTRING = cInput;
    _ns1__ENTRANCE_USCORETESTResponse m_ns1__ENTRANCE_USCORETESTResponse;
    BasicHttpBinding_USCOREIWSCenterEntranceProxy m_BasicHttpBinding_USCOREIWSCenterEntranceProxy2;
    int iReturnTest = -100;
    iReturnTest = m_BasicHttpBinding_USCOREIWSCenterEntranceProxy2.ENTRANCE_USCORETEST(WEB_SERVEICE_TEST_NULL,NULL,m_ns1__ENTRANCE_USCORETEST,m_ns1__ENTRANCE_USCORETESTResponse);
    QString sReturnTest = QString::fromLocal8Bit(m_ns1__ENTRANCE_USCORETESTResponse.ENTRANCE_USCORETESTResult);
//    qDebug()<<QString(tr("ENTRANCE_USCORETEST函数返回值: %1\n"
//                         "ENTRANCE_USCORETEST调用返回字符串:\n%2.\n")
//                      .arg(iReturnTest).arg(QString(sReturnTest)));
    emit web_Error_Msg(QString(tr("ENTRANCE_USCORETEST函数返回值: %1\nl1"
                                  "ENTRANCE_USCORETEST调用返回字符串:\n%2.\n")
                               .arg(iReturnTest).arg(sReturnTest)));
    delete m_ns1__ENTRANCE_USCORETEST;
}


/*************For Debug**************/
void WebUploadObj::forTest()//for debug
{

    QString V_duns_code = "ERP-0-10";
    QString V_library_action_code = "ICT_Board_Select_Add";
    QString V_action_code = "ICT_ManufacturerClassLibrary";
    QString V_input = QString("<DATA>"
                                 "<LINEALSNAME>%1</LINEALSNAME>"
                                 "<ARMNAME>%2</ARMNAME>"
                                 "<STATUS>%3</STATUS>"
                                 "<STARTTIME>%4</STARTTIME>"
                                 "<ENDTIME>%5</ENDTIME>"
                                 "<ERRORCODE>%6</ERRORCODE>"
                                 "</DATA>")
            .arg("自动化1").arg("1#").arg("F").arg("C").arg("2018-03-01 09:42:30").arg("Error PCB Unlevel on Flat");

    QString sDllFileName;
    //sDllFileName = "ACCMVC.dll";
    sDllFileName = "ClassLibrary6.dll";
    QLibrary webLib(sDllFileName);
    if(webLib.load())
    {
        qDebug()<<QString(tr("服务器DLL调用成功, 路径: %1.\n").arg(sDllFileName));
        emit web_Error_Msg(QString(tr("服务器DLL调用成功, 路径: %1.\n").arg(sDllFileName)));
//        DllFunc Call_Wscenter = (DllFunc)webLib.resolve("Call_Wscenter");
//        if(Call_Wscenter)
//        {
//            QString V_duns_code = "ERP-0-10";
//            QString V_library_action_code = "ICT_Board_Select_Add";
//            QString V_action_code = "ICT_ManufacturerClassLibrary";
//            QString V_input = QString("<DATA>"
//                                         "<LINEALSNAME>%1</LINEALSNAME>"
//                                         "<ARMNAME>%2</ARMNAME>"
//                                         "<STATUS>%3</STATUS>"
//                                         "<STARTTIME>%4</STARTTIME>"
//                                         "<ENDTIME>%5</ENDTIME>"
//                                         "<ERRORCODE>%6</ERRORCODE>"
//                                         "</DATA>")
//                    .arg("自动化1").arg("1#").arg("F").arg("2018-03-01 09:42:00").arg("2018-03-01 09:42:30").arg("Error PCB Unlevel on Flat");

//            Call_Wscenter(V_duns_code.toStdString(),V_library_action_code.toStdString(),V_action_code.toStdString(),V_input.toStdString());
//            qDebug()<<QString(tr("服务器DLL函数调用成功, 路径: %1.\n").arg(sDllFileName));
//            emit web_Error_Msg(QString(tr("服务器DLL函数调用成功, 路径: %1.\n").arg(sDllFileName)));
//        }
//        else {
//            qDebug()<<QString(tr("服务器DLL函数调用失败, 路径: %1.\n").arg(sDllFileName));
//            emit web_Error_Msg(QString(tr("服务器DLL函数调用失败, 路径: %1.\n").arg(sDllFileName)));
//        }
    }
    else {
        qDebug()<<QString(tr("服务器DLL调用失败, 路径: %1.\n").arg(sDllFileName));
        emit web_Error_Msg(QString(tr("服务器DLL调用失败, 路径: %1.\n").arg(sDllFileName)));
    }
}
