#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

private slots:
    void on_pushButton_moveToScan_clicked();

    void on_pushButton_Scan_clicked();

    void on_pushButton_pickUp_Carrier_clicked();

    void on_pushButton_place_Carrier_clicked();

    void on_pushButton_place_ICT_clicked();

    void on_pushButton_pickUp_ICT_clicked();

    void on_pushButton_ICTClose_clicked();

    void on_pushButton_ICTOpen_clicked();

    void on_pushButton_ICT_Run_clicked();

    void on_pushButton_place_OK_clicked();

    void on_pushButton_place_NG_clicked();

    void on_pushButton_cylinder_up_clicked();

    void on_pushButton_cylinder_down_clicked();

private:
    Ui::DebugDialog *ui;

signals:
    void moveToScan();
    void fixturePickup();
    void fixturePlace();
    void ictPlace();
    void ictPickup();
    void ictClose();
    void ictOpen();
    void placeOKPos();
    void placeNGPos();
};

#endif // DEBUGDIALOG_H
