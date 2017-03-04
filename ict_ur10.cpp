#include "ict_ur10.h"
#include "ui_ict_ur10.h"
#include "scannerdialog.h"
#include "robotdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include "staticname.h"

#include <QDebug>


ICT_UR10::ICT_UR10(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ICT_UR10)
{
    ui->setupUi(this);
//    ui->toolBar->setHidden(true);

    scan_thread = new ScannerThread(this);//实例scan线程对象
    robot_thread = new RobotThread(this);//实例robot线程对象

    scanner = new SerialPortWidget(this);
    robotServer = new TcpIpServer(this);
    robotServer->set_prefix_suffix("","\r\n");

    commDlg = new CommunicationDialog(this);
    commDlgIsShow = false;
    loginDlg = new LoginDialog(this);
    loginDlgIsShow = false;

    testCount = 0;

    ui->tableWidgetResultList->setColumnWidth(2,200);
    ui->tableWidgetResultList->setColumnWidth(3,200);

    connect(robot_thread,SIGNAL(startScan()),scan_thread,SLOT(scannerScanSN()));
    connect(robot_thread,SIGNAL(forShow(QString)),commDlg,SLOT(forShowInfo(QString)));

    connect(this,SIGNAL(forShow(QString)),commDlg,SLOT(forShowInfo(QString)));
    connect(this,SIGNAL(manualScan()),scan_thread,SLOT(scannerScanSN()));

    connect(scanner,SIGNAL(serialReadReady()),scan_thread,SLOT(scannerReadSN()));

    connect(scan_thread,SIGNAL(scanResult(QString)),this,SLOT(getSn(QString)));
    connect(scan_thread,SIGNAL(scanError(QString)),this,SLOT(errorMessage(QString)));
    connect(scan_thread,SIGNAL(forShow(QString)),commDlg,SLOT(forShowInfo(QString)));

    connect(robotServer,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(robotServer,SIGNAL(clientConnect(QString,int)),this,SLOT(robotConnected(QString,int)));
    connect(robotServer,SIGNAL(clientDisconnected(QString,int)),this,SLOT(robotDisconnected(QString,int)));
    connect(robotServer,SIGNAL(serverReadData(QString,int,QString)),robot_thread,SLOT(robotReadData(QString,int,QString)));

    init_Scanner_Robot();
    disEnableUI();
    newFile();

    scan_thread->start();
    robot_thread->start();
}

ICT_UR10::~ICT_UR10()
{
    if(scan_thread->isRunning())
        scan_thread->stop();
    if(robot_thread->isRunning())
        robot_thread->stop();
    QThread::msleep(100);
    delete scan_thread;
    delete robot_thread;
    delete ui;
}

void ICT_UR10::on_actionScanner_triggered()
{
    ScannerDialog scanDlg(this);
    scanDlg.exec();
}

void ICT_UR10::on_actionRobot_triggered()
{
    RobotDialog robotDlg(this);
    robotDlg.exec();

    ui->comboBoxTypeSelect->clear();
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString strTypeTemp = "";
    for(int i=0; i<COUNT; i++)
    {
        strTypeTemp = configRead->value(QString(MAIN_UI_TYPE).arg(i)).toString();
        if(""!=strTypeTemp)
        {
            ui->comboBoxTypeSelect->insertItem(i,strTypeTemp);
        }
    }
    delete configRead;
}

void ICT_UR10::on_actionCommunication_triggered()
{
    if(false == commDlgIsShow)
    {
        commDlg->show();
        commDlgIsShow = true;
    }
    else
    {
        commDlg->hide();
        commDlgIsShow = false;
    }
}

void ICT_UR10::init_Scanner_Robot()
{
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);

    //Scanner
    QString portName = configRead->value(SCANNER_PORT_NAME).toString();
    int baudRate = configRead->value(SCANNER_BAUD_RATE).toString().toInt();
    int dataBits = configRead->value(SCANNER_DATA_BITS).toString().toInt();
    QString parityBits = configRead->value(SCANNER_PARITY_BITS).toString();
    QString stopBits = configRead->value(SCANNER_STOP_BITS).toString();
    if(!(scanner->openSerialPort(portName,baudRate,dataBits,parityBits,stopBits,true,true)))
    {
        QMessageBox::warning(this,tr("Error"),tr("Scanner initialize failed!"),QMessageBox::Ok);
        ui->labelScanStatus->setText("Scanner:Disconnected!");
        emit forShow(forShowString("Scanner:Disconnected!"));
    }
    else
    {
        ui->labelScanStatus->setText(QString("Scanner:%1 Connected").arg(portName));
        emit forShow(forShowString(QString("Scanner:%1 Connected").arg(portName)));
    }

    //Robot
    QString address =configRead->value(SERVER_IP_ADDRESS).toString();
    quint16 port =(quint16) configRead->value(SERVER_PORT).toString().toInt();
    if(!robotServer->stratListen(address,port))
    {
        QMessageBox::warning(this,tr("Error"),tr("Robot initialize failed!"),QMessageBox::Ok);
        ui->labelRobotStatus->setText("Robot:Disconnected!");
        emit forShow(forShowString("Robot:Disconnected!"));
    }
    else
    {
        ui->labelRobotStatus->setText("Robot:Listening...");
        emit forShow(forShowString("Robot:Listening..."));
    }

    ui->comboBoxTypeSelect->clear();
    QString strTypeTemp = "";
    for(int i=0; i<COUNT; i++)
    {
        strTypeTemp = configRead->value(QString(MAIN_UI_TYPE).arg(i)).toString();
        if(""!=strTypeTemp)
        {
            ui->comboBoxTypeSelect->insertItem(i,strTypeTemp);
        }
    }

    delete configRead;
}

void ICT_UR10::getSn(QString sn)
{
    if(ui->lineEditSN->text().isEmpty())
    {
        sn.replace("\r","");
        sn.replace("\n","");
        ui->lineEditSN->setText(sn);
    }
}

void ICT_UR10::manualStartScan()
{
    emit manualScan();
}

void ICT_UR10::errorMessage(QString errorMsg)
{
    QMessageBox::warning(this,tr("Error"),QString("%1").arg(errorMsg),QMessageBox::Ok);
}

void ICT_UR10::on_actionLogin_triggered()
{
    if(false == loginDlgIsShow)
    {
        loginDlg->show();
        loginDlgIsShow = true;
    }
    else
    {
        loginDlg->close();
        loginDlgIsShow = false;
        disEnableUI();
    }
}

void ICT_UR10::disEnableUI()
{
    ui->actionLogin->setText("LogIn");
    ui->actionScanner->setDisabled(true);
    ui->actionRobot->setDisabled(true);
    commDlg->disEnable();
}

void ICT_UR10::Enable()
{
    ui->actionLogin->setText("LogOut");
    ui->actionScanner->setDisabled(false);
    ui->actionRobot->setDisabled(false);
    commDlg->Enable();
}

void ICT_UR10::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::warning(this,tr("Warning"),tr("Are you sure to exit!"),QMessageBox::Yes|QMessageBox::No);

    if (button == QMessageBox::No)
    {
        event->ignore();  //ignore
    }
    else if (button == QMessageBox::Yes)
    {
        event->accept();  //accept
    }
}

void ICT_UR10::robotConnected(QString IP, int Port)
{
    emit forShow(forShowString(QString("Robot:%1 %2 Connected").arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP==IP && robotPort==QString("%1").arg(Port))
    {
        ui->labelRobotStatus->setText(QString("Robot:%1 %2 Connected").arg(IP).arg(Port));
    }
}

void ICT_UR10::robotDisconnected(QString IP, int Port)
{
    emit forShow(forShowString(QString("Robot:%1 %2 Disconnected!").arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP==IP && robotPort==QString("%1").arg(Port))
    {
        ui->labelRobotStatus->setText(QString("Robot:%1 %2 Disconnected!").arg(IP).arg(Port));
    }
}

QString ICT_UR10::forShowString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + str + "\r\n";
    return str;
}

void ICT_UR10::updateScannerStatue(QString portName,bool connected)
{
    if(true == connected)
    {
        ui->labelScanStatus->setText(QString("Scanner:%1 Connected").arg(portName));
        emit forShow(forShowString(QString("Scanner:%1 Connected").arg(portName)));
        return;
    }
}

void ICT_UR10::updateTestResult(QString sn, QString result)
{
    if(100<=testCount)
    {
        ui->tableWidgetResultList->clearContents();
        ui->tableWidgetResultList->setRowCount(0);
    }
    testCount++;

    ui->tableWidgetResultList->setRowCount(ui->tableWidgetResultList->rowCount() + 1);
    QDateTime time = QDateTime::currentDateTime();
    int row = ui->tableWidgetResultList->rowCount() - 1;
    ui->tableWidgetResultList->setItem(row,0,new QTableWidgetItem(time.toString("yyyyMMdd")));
    ui->tableWidgetResultList->setItem(row,1,new QTableWidgetItem(time.toString("hh:mm:ss")));
    ui->tableWidgetResultList->setItem(row,2,new QTableWidgetItem(sn));
    ui->tableWidgetResultList->setItem(row,3,new QTableWidgetItem(result));
    ui->tableWidgetResultList->item(row, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 1)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 2)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 3)->setTextAlignment(Qt::AlignCenter);

    if("PASS"==result.toUpper())
    {
        ui->tableWidgetResultList->item(row, 3)->setBackgroundColor(QColor(0, 255, 0));
    }
    else
    {
        if("FAIL"==result.toUpper())
        {
            ui->tableWidgetResultList->item(row, 3)->setBackgroundColor(QColor(255, 0, 0));
        }
    }

    ui->tableWidgetResultList->scrollToBottom();

    QString datalist = QString("%1,%2,%3,%4\n").arg(time.toString("yyyyMMdd"))
            .arg(time.toString("hh:mm:ss")).arg(sn).arg(result);
    newFile();//检查本地数据文件夹是否存在，如果不存在则新建文件夹
    QFile csvFile(QString(LOCAL_DATA_FILE_NAME).arg(time.toString("yyyyMMdd")));
    if (csvFile.open(QIODevice::ReadWrite))
    {
        if(0 == csvFile.size())
        {
            csvFile.write(QString("Date,Time,SN,TestResult\n").toLatin1());
        }
        csvFile.seek(csvFile.size());
        csvFile.write(datalist.toLatin1());
        csvFile.close();
    }
}

void ICT_UR10::newFile()
{
    QDir *temp = new QDir;
    bool fileExist = temp->exists(LOCAL_DATA_FOLDER_NAME);
    if(!fileExist)
    {
        temp->mkdir(LOCAL_DATA_FOLDER_NAME);
    }
}
