#ifndef COMMUNICATIONDIALOG_H
#define COMMUNICATIONDIALOG_H

#include <QDialog>

namespace Ui {
class CommunicationDialog;
}

class CommunicationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommunicationDialog(QWidget *parent = 0);
    ~CommunicationDialog();

private slots:
    void on_pushButtonSend_clicked();

    void forShowInfo(QString msg);

private:
    Ui::CommunicationDialog *ui;
};

#endif // COMMUNICATIONDIALOG_H
