#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QCloseEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_pushButtonLogin_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::LoginDialog *ui;
    void closeEvent(QCloseEvent *event);
};

#endif // LOGINDIALOG_H
