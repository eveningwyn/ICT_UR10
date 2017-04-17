#include "communicationdialog.h"
#include "ui_communicationdialog.h"
#include "staticname.h"
#include "ict_ur10.h"
#include "language.h"
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

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
        //ptr->manualStartScan(false);
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
    saveInfoToFile(msg);
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
    checkFileExist();
    QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
    QFile file(QString(INFORMATION_FILE_NAME).arg(date));
    if(file.open(QFile::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);    // 鼠标指针变为等待状态
        out << msg;
        QApplication::restoreOverrideCursor();              // 鼠标指针恢复原来的状态
        file.close();
    }
}

void CommunicationDialog::checkFileExist()
{
    QDir *temp = new QDir;
    bool fileExist = temp->exists(INFORMATION_FOLDER_NAME);
    if(!fileExist)
    {
        temp->mkdir(INFORMATION_FOLDER_NAME);
    }
    delete temp;
}
