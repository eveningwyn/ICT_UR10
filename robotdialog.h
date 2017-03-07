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

    void on_pushButtonAddType_clicked();

    void on_pushButtonDelType_clicked();

    void on_pushButtonSaveType_clicked();

private:
    Ui::RobotDialog *ui;
    void initGetInfo();//初始化UI
    bool isSave;
    void saveConfig();//保存参数配置
};

#endif // ROBOTDIALOG_H
