﻿#include "communicationdialog.h"
#include "ui_communicationdialog.h"
#include "staticname.h"
#include "ict_ur10.h"
#include "language.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QSettings>
#include <QException>

CommunicationDialog::CommunicationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommunicationDialog)
{
    ui->setupUi(this);
}

CommunicationDialog::~CommunicationDialog()
{
    delete ui;
}

void CommunicationDialog::on_pushButtonSend_clicked()
{
    QString str = ui->lineEditSendData->text();
    if(str.isEmpty())
    {
        return;
    }

    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    if(true == ptr->isAutoRun)
    {
        ui->lineEditSendData->clear();
        return;//如果正在自动运行中，则取消发送
    }

    if("scanner" == ui->comboBoxReceiver->currentText().toLower())
    {
        QMessageBox::StandardButton rb = QMessageBox::warning(this,NULL,
                                                              tr("在线扫描请选择Yes，离线扫描请选择No，取消请选择Cancel."),
                                                              QMessageBox::Cancel|QMessageBox::No|QMessageBox::Yes);
        if(rb==QMessageBox::Yes)
        {
            ptr->manualStartScan(true);
        }
        else
        {
            if(rb==QMessageBox::No)
            {
                ptr->manualStartScan(false);
            }
        }
    }
    else
    {
        if("robot" == ui->comboBoxReceiver->currentText().toLower())
        {
            QString suffix = ui->comboBoxSuffix->currentText();
            suffix.replace("\\r","\r");
            suffix.replace("\\n","\n");
            str += suffix;
            ptr->manualSendMsg_robot(str);
        }
        else
        {
            if("controlboard" == ui->comboBoxReceiver->currentText().toLower())
            {
                emit ptr->manualSendMsg_controlBoard(str);
            }
        }
    }
}

void CommunicationDialog::forShowInfo(QString msg)
{
    try
    {
        saveInfoToFile(msg);
    }
    catch(QException /*&ex*/)
    {
        qDebug("please return this error feedback to the developers");
    }
    if((!(ui->checkBoxShowInfo->isChecked())) || "UI clear...\n"==msg)
    {
        ui->textBrowserCommunication->clear();
    }
    ui->textBrowserCommunication->moveCursor(QTextCursor::End);
    ui->textBrowserCommunication->insertPlainText(msg);
    ui->textBrowserCommunication->moveCursor(QTextCursor::End);
}

void CommunicationDialog::closeEvent(QCloseEvent *event)
{
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    ptr->commDlgIsShow = false;
//        close();
    event->accept();  //accept
}

void CommunicationDialog::disEnable(bool disable)
{
    ui->pushButtonSend->setDisabled(disable);
}

void CommunicationDialog::on_comboBoxReceiver_currentTextChanged(const QString &arg1)
{
    if("Scanner" == arg1)
    {
        ui->lineEditSendData->setText("<T>");
        ui->lineEditSendData->setDisabled(true);
        ui->comboBoxSuffix->setCurrentIndex(0);
    }
    else
    {
        ui->lineEditSendData->clear();
        ui->lineEditSendData->setDisabled(false);
    }
}

void CommunicationDialog::saveInfoToFile(QString msg)
{
    static QMutex comm_mutex;
    comm_mutex.lock();
    try
    {
        QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
        checkFileExist(INFORMATION_FOLDER_NAME);
        checkFileExist(QString("%1/%2").arg(INFORMATION_FOLDER_NAME).arg(date));
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString logIndex = configRead->value(LOG_INDEX).toString();
        delete configRead;
        QFile file(QString(INFORMATION_FILE_NAME).arg(date).arg(logIndex));
        if(file.open(QFile::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QTextStream out(&file);
            QApplication::setOverrideCursor(Qt::WaitCursor);    // 鼠标指针变为等待状态
            out << msg;
            QApplication::restoreOverrideCursor();              // 鼠标指针恢复原来的状态
            file.close();
        }
    }
    catch(QException /*&ex*/)
    {
        qDebug("please return this error feedback to the developers");
    }
    comm_mutex.unlock();
}

void CommunicationDialog::checkFileExist(QString fileName)
{
    QDir *temp = new QDir;
    bool fileExist = temp->exists(fileName);
    if(!fileExist)
    {
        temp->mkdir(fileName);
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        configRead->setValue(LOG_INDEX,"0");
        delete configRead;
    }
    delete temp;
}
