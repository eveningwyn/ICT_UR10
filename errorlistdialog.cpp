#include "errorlistdialog.h"
#include "ui_errorlistdialog.h"
#include <QDateTime>
#include "ict_ur10.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

ErrorListDialog::ErrorListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorListDialog)
{
    ui->setupUi(this);
}

ErrorListDialog::~ErrorListDialog()
{
    delete ui;
}

void ErrorListDialog::errorMessage(QString errorMsg)
{
    QDateTime time = QDateTime::currentDateTime();
    errorMsg = time.toString("yyyy-MM-dd hh:mm:ss.zzz :") + "\n" + errorMsg;
    ui->textBrowserErrorList->moveCursor(QTextCursor::End);
    ui->textBrowserErrorList->insertPlainText(errorMsg);
    ui->textBrowserErrorList->moveCursor(QTextCursor::End);
}

void ErrorListDialog::closeEvent(QCloseEvent *event)
{
    ICT_UR10 *ptr = (ICT_UR10*)parentWidget();
    ptr->errorDlgIsShow = false;
//        close();
    event->accept();  //accept
}

void ErrorListDialog::on_pushButtonSaveErrorList_clicked()
{
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd_hh-mm-ss");
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save As..."),QString("ErrorList_%1.txt").arg(strTime));
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QFile::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            QApplication::setOverrideCursor(Qt::WaitCursor);    // 鼠标指针变为等待状态
            out << ui->textBrowserErrorList->toPlainText();
            QApplication::restoreOverrideCursor();              // 鼠标指针恢复原来的状态
            file.close();
        }
    }
}

void ErrorListDialog::on_pushButtonClearErrorList_clicked()
{
    if(QMessageBox::Yes==QMessageBox::warning(this,"Warning","Are you sure you don't need to save error records?",QMessageBox::Yes|QMessageBox::No))
        ui->textBrowserErrorList->clear();
}

void ErrorListDialog::disEnable()
{
    ui->pushButtonSaveErrorList->setDisabled(true);
    ui->pushButtonClearErrorList->setDisabled(true);
}

void ErrorListDialog::Enable()
{
    ui->pushButtonSaveErrorList->setDisabled(false);
    ui->pushButtonClearErrorList->setDisabled(false);
}
