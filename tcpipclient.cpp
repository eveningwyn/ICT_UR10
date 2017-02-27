#include "tcpipclient.h"
#include <QHostAddress>

TcpIpClient::TcpIpClient(QObject *parent) :
    QTcpSocket(parent)
{
    clientID = -1;
    readStrMsg = "";

    connect(this,SIGNAL(readyRead()),this,SLOT(clientReadData()));
    connect(this,SIGNAL(disconnected()),this,SLOT(DisConnect()));//关闭连接时，发送断开连接信号
    connect(this,SIGNAL(disconnected()),this,SLOT(deleteLater()));//关闭连接时，对象自动删除
}

void TcpIpClient::clientSendData(QString msg)
{
    QByteArray sendByte = msg.toLatin1();
    this->write(sendByte);
}

void TcpIpClient::clientReadData()
{
    QByteArray readByte = this->readAll();
    readStrMsg = QString(readByte);
    emit readData(this->clientID,this->peerAddress().toString(),this->peerPort(),readStrMsg);
    readStrMsg = "";
}

void TcpIpClient::DisConnect()
{
    //断开连接时，发送断开信号
    emit clientDisConnect(this->clientID,this->peerAddress().toString(),this->peerPort());
}

void TcpIpClient::newConnect(const QString address, quint16 port)
{
//    this->abort();//关闭已有连接
    this->connectToHost(QHostAddress(address),port);
    if (!this->waitForConnected(3000))
        emit cliendErrorMsg(this->errorString());
}

void TcpIpClient::closeConnect()
{
    this->disconnectFromHost();
//    if(!(this->state() == QAbstractSocket::UnconnectedState || this->waitForDisconnected(1000)))
//        emit cliendErrorMsg(this->errorString());
}

bool TcpIpClient::bindClient(const QString address, quint16 port)
{
    QHostAddress addr(address);
    if(this->bind(addr, port, DefaultForPlatform))
        return true;

    return false;
}
