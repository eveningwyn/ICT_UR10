#include "communicationdialog.h"
#include "ui_communicationdialog.h"
#include "ict_ur10.h"
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

    if("scanner" == ui->comboBoxReceiver->currentText().toLower())
    {
//        ptr->scanner->serialPortWrite(str);
        ptr->manualStartScan();
    }
    else
        if("robot" == ui->comboBoxReceiver->currentText().toLower())
        {
            QString suffix = ui->comboBoxSuffix->currentText();
            suffix.replace("\\r","\r");
            suffix.replace("\\n","\n");
            str += suffix;
            ptr->robot_thread->robotManualSendMsg(str);
        }
}

void CommunicationDialog::forShowInfo(QString msg)
{
    if(!(ui->checkBoxShowInfo->isChecked()))
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

void CommunicationDialog::disEnable()
{
    ui->pushButtonSend->setDisabled(true);
}

void CommunicationDialog::Enable()
{
    ui->pushButtonSend->setDisabled(false);
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
