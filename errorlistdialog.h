#ifndef ERRORLISTDIALOG_H
#define ERRORLISTDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QMutex>

namespace Ui {
class ErrorListDialog;
}

class ErrorListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorListDialog(QWidget *parent = 0);
    ~ErrorListDialog();
    void disEnable(bool disable);

private:
    Ui::ErrorListDialog *ui;
    void closeEvent(QCloseEvent *event);
    void saveErrorToFile(QString errorMsg);
    void checkFileExist(QString fileName);

    QMutex error_mutex;

public slots:
    void recordErrorMessage(QString errorMsg);//错误记录清单

private slots:
    void on_pushButtonSaveErrorList_clicked();
    void on_pushButtonClearErrorList_clicked();
};

#endif // ERRORLISTDIALOG_H
