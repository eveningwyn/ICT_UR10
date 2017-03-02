﻿#include "tcpipserver.h"
#include <QMessageBox>

TcpIpServer::TcpIpServer(QObject *parent):
    QTcpServer(parent)
{
    clientSocketList.clear();//清空客户端对象集合
    clientSocketID.clear();//清空客户端序号集合
    prefix = "";
    suffix = "";
}

void TcpIpServer::incomingConnection(qintptr socketDescriptor)
{
    TcpIpClient *clientSocket=new TcpIpClient(this);
    if (!clientSocket->setSocketDescriptor(socketDescriptor))
    {
        emit errorMessage(clientSocket->errorString());
        return;
    }
    clientSocket->clientID = socketDescriptor;//配置客户端对象ID
    clientSocket->prefix = this->prefix;
    clientSocket->suffix = this->suffix;

    connect(clientSocket,SIGNAL(readData(int,QString,int,QString)),this,SLOT(serverReadMsg(int,QString,int,QString)));
    connect(clientSocket,SIGNAL(clientDisConnect(int,QString,int)),this,SLOT(disConnect(int,QString,int)));

    emit clientConnect(clientSocket->peerAddress().toString(),clientSocket->peerPort());//发送已连接的客户端信息

    clientSocketList.append(clientSocket);//将新的客户端连接对象添加到列表
    clientSocketID.append(socketDescriptor);//将新的客户端连接对象ID添加到列表
}

void TcpIpServer::serverReadMsg(int clientID,QString IP,int Port,QString readMsg)
{
    clientID = 0;//此处不需此参数
    emit serverReadData(IP,Port,readMsg);
}

void TcpIpServer::disConnect(int clientID,QString IP,int Port)
{
    int i=0;
    for (;i<clientSocketID.count();i++)
    {
        if(clientSocketList[i]->peerAddress().toString()==IP)
        {}
        if(clientSocketList[i]->peerPort()==Port)
        {}
        if (clientSocketID[i]==clientID)
        {
            clientSocketList.removeAt(i);//从列表中移除该连接
            emit clientDisconnected(IP,Port);
            break;
        }
    }
    clientSocketID.removeAt(i);
}

//指定客户端连接发消息
void TcpIpServer::sendData(quint16 port, QString sendMsg)
{
    QByteArray sendByte = sendMsg.toLatin1();
    for (int i=0;i<clientSocketID.count();i++)
    {
        if (clientSocketList[i]->peerPort()==port)
        {
            clientSocketList[i]->write(sendByte);
            break;
        }
    }
}

bool TcpIpServer::stratListen(QString address,quint16 port)
{   //IP地址 (e.g., "127.0.0.1").
    if(!this->isListening())
    {
        QHostAddress addr(address);
        if(!this->listen(addr,port))
        {
            emit errorMessage(this->errorString());
            this->close();
            return false;
        }
    }
    return true;
}

void TcpIpServer::closeServerListen()
{
    this->close();
}

int TcpIpServer::getClientID(QString IP, int Port)
{
    int i=0;
    for (;i<clientSocketID.count();i++)
    {
        if(clientSocketList[i]->peerPort()==Port && clientSocketList[i]->peerAddress().toString()==IP)
        {
            return clientSocketID[i];
        }
    }
    return -1;
}

void TcpIpServer::set_prefix_suffix(QString prefix, QString suffix)
{
    this->prefix = prefix;
    this->suffix = suffix;
}
