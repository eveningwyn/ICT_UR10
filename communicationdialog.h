#ifndef COMMUNICATIONDIALOG_H
#define COMMUNICATIONDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QMutex>

namespace Ui {
class CommunicationDialog;
}

class CommunicationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommunicationDialog(QWidget *parent = 0);
    ~CommunicationDialog();
    void disEnable(bool disable);

public slots:
    void forShowInfo(QString msg);//显示信息到界面
    void on_comboBoxReceiver_currentTextChanged(const QString &arg1);

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::CommunicationDialog *ui;
    void closeEvent(QCloseEvent *event);
    void saveInfoToFile(QString msg);
    void checkFileExist(QString fileName);
    QMutex comm_mutex;
};

#endif // COMMUNICATIONDIALOG_H
