#ifndef WEBUPLOADOBJ_H
#define WEBUPLOADOBJ_H

#include <QObject>
#include <QtNetwork>

class QNetworkAccessManager;
class QNetworkRequest;

class WebUploadObj : public QObject
{
    Q_OBJECT
public:
    explicit WebUploadObj(QObject *parent = 0);
    ~WebUploadObj();

    void forTest();//For Debug

private:
    QNetworkAccessManager *web_manager;
    QNetworkRequest web_request;
//    QNetworkReply *web_reply;
    QString sUID;
    void postWeb(const QString postMsg);//Old Code
    void callWebService(const QString inputStr);
    void callWebServiceTest(const QString inputStr);
    QString getXmlString(const QString state, const QString startTime, const QString endTime, const QString errorCode);

signals:
    void web_Error_Msg(QString errorMsg);//更新错误信号

public slots:
    void init_web();
    void msgUpload(const QString state, const QString startTime, const QString endTime, const QString errorCode);

private slots:
    void replyFinished(QNetworkReply *reply);

};

#endif // WEBUPLOADOBJ_H
