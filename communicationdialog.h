#ifndef COMMUNICATIONDIALOG_H
#define COMMUNICATIONDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class CommunicationDialog;
}

class CommunicationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommunicationDialog(QWidget *parent = 0);
    ~CommunicationDialog();
    void disEnable();
    void Enable();

public slots:
    void forShowInfo(QString msg);//显示信息到界面
    void on_comboBoxReceiver_currentTextChanged(const QString &arg1);

private slots:
    void on_pushButtonSend_clicked();

private:
    Ui::CommunicationDialog *ui;
    void closeEvent(QCloseEvent *event);
};

#endif // COMMUNICATIONDIALOG_H
