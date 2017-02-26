#ifndef ROBOTDIALOG_H
#define ROBOTDIALOG_H

#include <QDialog>

namespace Ui {
class RobotDialog;
}

class RobotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RobotDialog(QWidget *parent = 0);
    ~RobotDialog();

private slots:
    void on_Setting_clicked();

private:
    Ui::RobotDialog *ui;
    void initialize();
};

#endif // ROBOTDIALOG_H
