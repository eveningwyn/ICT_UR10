#include "errorlistdialog.h"
#include "ui_errorlistdialog.h"
#include <QDateTime>
#include "ict_ur10.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include "language.h"
#include "staticname.h"

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

void ErrorListDialog::recordErrorMessage(QString errorMsg)
{
    QDateTime time = QDateTime::currentDateTime();
    errorMsg = time.toString("yyyy-MM-dd hh:mm:ss.zzz :") + "\n" + errorMsg;
    ui->textBrowserErrorList->moveCursor(QTextCursor::End);
    ui->textBrowserErrorList->insertPlainText(errorMsg);
    ui->textBrowserErrorList->moveCursor(QTextCursor::End);
    saveErrorToFile(errorMsg);
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
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为..."),QString("ErrorList_%1.txt").arg(strTime));
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
    if(QMessageBox::Yes==QMessageBox::warning(this,tr("提示"),tr("请确认是否清空错误记录？"),QMessageBox::Yes|QMessageBox::No))
        ui->textBrowserErrorList->clear();
}

void ErrorListDialog::disEnable(bool disable)
{
    ui->pushButtonSaveErrorList->setDisabled(disable);
    ui->pushButtonClearErrorList->setDisabled(disable);
}

void ErrorListDialog::saveErrorToFile(QString errorMsg)
{
    checkFileExist();
    QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
    QFile file(QString(ERROR_FILE_NAME).arg(date));
    if(file.open(QFile::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(&file);
        QApplication::setOverrideCursor(Qt::WaitCursor);    // 鼠标指针变为等待状态
        out << errorMsg;
        QApplication::restoreOverrideCursor();              // 鼠标指针恢复原来的状态
        file.close();
    }
}

void ErrorListDialog::checkFileExist()
{
    QDir *temp = new QDir;
    bool fileExist = temp->exists(INFORMATION_FOLDER_NAME);
    if(!fileExist)
    {
        temp->mkdir(INFORMATION_FOLDER_NAME);
    }
}
