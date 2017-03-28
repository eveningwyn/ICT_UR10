#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

public slots:
    void runDone();

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

    void on_pushButton_return_clicked();

    void on_pushButton_ClawOpen_clicked();

    void on_pushButton_ClawClose_clicked();

private:
    Ui::DebugDialog *ui;
    void closeEvent(QCloseEvent *event);
    void urRunning();

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
    void returnSafePos();
    void clawOpen();
    void clawClose();
    void ict_start_test();
    void debug_CylinderUpDown(QString str);
};

#endif // DEBUGDIALOG_H
