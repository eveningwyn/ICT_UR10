#ifndef SCANNERDIALOG_H
#define SCANNERDIALOG_H

#include <QDialog>

namespace Ui {
class ScannerDialog;
}

class ScannerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScannerDialog(QWidget *parent = 0);
    ~ScannerDialog();

private slots:

    void on_pushButtonSetting_clicked();

    void on_pushButtonRefresh_clicked();

private:
    Ui::ScannerDialog *ui;
    void initializeInfo();
    void getPortName();
};

#endif // SCANNERDIALOG_H
