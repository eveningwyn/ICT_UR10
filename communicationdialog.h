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


private slots:
    void on_pushButtonSend_clicked();

    void forShowInfo(QString msg);

    void on_comboBoxReceiver_currentTextChanged(const QString &arg1);

private:
    Ui::CommunicationDialog *ui;
    void closeEvent(QCloseEvent *event);
};

#endif // COMMUNICATIONDIALOG_H
