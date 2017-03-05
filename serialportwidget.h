﻿#ifndef SERIALPORTWIDGET_H
#define SERIALPORTWIDGET_H

/*需要在.pro文件添加QT += serialport*/
#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPortWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortWidget(QWidget *parent = 0);
    ~SerialPortWidget();

    bool openSerialPort(const QString portName,int baudRate,
                        int dataBit,QString parityBit,QString stopBit,bool setDTR,bool setRTS);//参数：端口号、波特率、数据位、校验位、停止位、DTR和RTS
    void closeSerialPort();
    void setDTR_RTS(bool setDTR,bool setRTS);
    void serialPortRead(QString &readString,QString prefix,QString suffix);
    void serialPortWrite(QString writeString);
    void clearBuffer();

signals:
    void serialReadReady();

public slots:

private slots:

private:
    QSerialPort *serial;
    QByteArray byteRead;    //存取串口读取的全部数据

    void setPortName(const QString &portName);
    void setBaudRate(int baudRate);
    void setDataBits(int dataBit);
    void setParity(QString parityBit);
    void setStopBits(QString stopBit);
};

#endif // SERIALPORTWIDGET_H
