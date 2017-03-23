#ifndef ICT_MES_DIALOG_H
#define ICT_MES_DIALOG_H

#include <QDialog>

namespace Ui {
class ICT_MES_Dialog;
}

class ICT_MES_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit ICT_MES_Dialog(QWidget *parent = 0);
    ~ICT_MES_Dialog();

public slots:

private slots:
    void on_pushButton_Save_ICT_Config_clicked();

    void on_checkBox_ICT_Enable_clicked();

private:
    Ui::ICT_MES_Dialog *ui;
    void saveICTConfig();
};

#endif // ICT_MES_DIALOG_H
