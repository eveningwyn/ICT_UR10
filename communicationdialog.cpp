#include "communicationdialog.h"
#include "ui_communicationdialog.h"
#include "ict_ur10.h"

CommunicationDialog::CommunicationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommunicationDialog)
{
    ui->setupUi(this);
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    connect(ptr->scan_thread,SIGNAL(forShow(QString)),this,SLOT(forShowInfo(QString)));
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
//            ptr->scanner->serialPortWrite(str);
        }
}

void CommunicationDialog::forShowInfo(QString msg)
{
    ui->textBrowserCommunication->moveCursor(QTextCursor::End);
    ui->textBrowserCommunication->insertPlainText(msg);
    ui->textBrowserCommunication->moveCursor(QTextCursor::End);
}
