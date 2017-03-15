#include "mescheckobj.h"
#include "language.h"
#include "staticname.h"
#include <QUuid>
#include <QtXml>

#include <QDebug>

MesCheckObj::MesCheckObj(QObject *parent) : QObject(parent)
{

}

void MesCheckObj::init_mes()
{
    V_UID = QUuid::createUuid().toString();//取得唯一的GUID码
    V_USER_ID = "SF";

    mesEnable = true;
    getConfig = false;
    mes_manager = new QNetworkAccessManager(this);
    connect(mes_manager,&QNetworkAccessManager::finished,this,&MesCheckObj::replyFinished);
}

void MesCheckObj::checkSn(QString sn)
{
    if(true == mesEnable)
    {
        if(true)
        {
            emit checkSnResult(sn,true);
        }
        else
        {
            emit checkSnResult(sn,false);
            emit mes_Error_Msg(tr("当前产品不属于当前工站，请确认该产品已正常过站！\n"));
        }
    }
    else
    {
        emit checkSnResult(sn,true);
    }
}

void MesCheckObj::set_mes_enable(bool enable)
{
    mesEnable = enable;
}

void MesCheckObj::get_mes_config(QString strMsg)
{
    getConfig = true;
    post_mes(V_UID, V_USER_ID, GET_CONFIG, strMsg);
}

void MesCheckObj::get_mes_SN_info(QString sn)
{
    QString V_ChickIn =QString("<PN>%1</PN>"
                               "<WSID>%2</WSID>"
                               "<TPVER>%3</TPVER>"
                               "<MFGSITE>%4</MFGSITE>"
                               "<SFIS>%5</SFIS>"
                               "<LINE>%6</LINE>"
                               "<SN>%7</SN>"
                               "<OEM_GROUP>%8</OEM_GROUP>")
            .arg("S0S61PCPU005A").arg("AOI").arg("01").arg("JOYTECH").arg("1").arg("S01").arg(sn).arg("61");
    post_mes(V_UID, V_USER_ID, CHECKIN, V_ChickIn);
}

void MesCheckObj::post_mes(QString V_UID, QString V_USER_ID, QString V_LIBRARY_ACTION_CODE, QString V_INPUT)
{
//    QUrl url(WEB_SERVEICE);
    QUrl url(WEB_SERVEICE_TEST);//测试网络
    mes_request.setUrl(url);
    QByteArray post_data = "";
    QString intputString = "";

    intputString = QString("<INPUTSTRING>"
                           "<DUNS_CODE>%1</DUNS_CODE>"
                           "<UID>%2</UID>"
                           "<USER_ID>%3</USER_ID>"
                           "<IS_ONE_WAY>FALSE</IS_ONE_WAY>"
                           "<OUTPUT_FORMAT>XML</OUTPUT_FORMAT>"
                           "<SP_NAME></SP_NAME>"
                           "<ACTION_CODE>ShopfloorClassLibrary</ACTION_CODE>"
                           "<LIBRARY_ACTION_CODE>%4</LIBRARY_ACTION_CODE>"
                           "<ROWSET><INPUT>%5</INPUT></ROWSET>"
                           "</INPUTSTRING>")
            .arg("SF-TEST").arg(V_UID).arg(V_USER_ID).arg(V_LIBRARY_ACTION_CODE).arg(V_INPUT);

    post_data = intputString.toLatin1();
    mes_manager->post(mes_request,post_data);
}

void MesCheckObj::replyFinished(QNetworkReply *reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if(QNetworkReply::NoError == reply->error())
    {
        QByteArray replyBytes = reply->readAll();

        QString replyStr(replyBytes);//转化为字符串
        qDebug()<<replyStr;

        QDomDocument doc;//创建Dom类对象解析xml
        if(doc.setContent(replyStr))
        {
            QDomNode node = doc.documentElement().elementsByTagName("ROWSET").at(0);
            node = node.toElement().elementsByTagName("OUTPUT").at(0);
            replyStr = node.toElement().text();//得到xml中某个节点的值
        }

        if(true == getConfig)
        {
            emit rtnConfig(replyStr);
        }
    }
    else
    {
        //处理错误
        emit mes_Error_Msg(QString(tr("MES应答信息异常！\n%1\n")).arg(reply->errorString()));
    }

    getConfig = false;
    reply->deleteLater();
}
