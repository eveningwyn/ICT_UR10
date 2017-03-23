#ifndef DEBUGLOGINDIALOG_H
#define DEBUGLOGINDIALOG_H

#include <QDialog>

namespace Ui {
class DebugLoginDialog;
}

class DebugLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugLoginDialog(QWidget *parent = 0);
    ~DebugLoginDialog();

private slots:
    void on_pushButton_Login_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::DebugLoginDialog *ui;
};

#endif // DEBUGLOGINDIALOG_H
