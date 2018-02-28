#ifndef WEBUPLOADOBJ_H
#define WEBUPLOADOBJ_H

#include <QObject>
#include <QtNetwork>
#include <QLibrary>

class QNetworkAccessManager;
class QNetworkRequest;

class WebUploadObj : public QObject
{
    Q_OBJECT
public:
    explicit WebUploadObj(QObject *parent = 0);
    ~WebUploadObj();
    void msgUpload(const QString state, const QString startTime, const QString endTime, const QString errorCode);
    void forTest();

private:
    QNetworkAccessManager *web_manager;
    QNetworkRequest web_request;
//    QNetworkReply *web_reply;
    QString V_UID;
    void postWeb(const QString postMsg);
    void postWebNew(const QString postMsg);
    void callWebFunc(QString intputMsg);

signals:
    void web_Error_Msg(QString errorMsg);//更新错误信号

public slots:
    void init_web();

private slots:
    void replyFinished(QNetworkReply *reply);

};

#endif // WEBUPLOADOBJ_H
