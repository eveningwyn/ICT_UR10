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

    totalQty = 0;
    passQty = 0;
    failQty = 0;
    yield = 0.00;

    scan_thread = new ScannerThread(this);//实例scan线程对象
    robot_thread = new RobotThread(this);//实例robot线程对象

    scanner = new SerialPortWidget(this);
    robotServer = new TcpIpServer(this);
    robotServer->set_prefix_suffix("","\r\n");

    commDlg = new CommunicationDialog(this);
    commDlgIsShow = false;
    loginDlg = new LoginDialog(this);
    loginDlgIsShow = false;
    errorDlg = new ErrorListDialog(this);
    errorDlgIsShow = false;

    testCount = 0;

    ui->tableWidgetResultList->setColumnWidth(3,200);

    connect(robot_thread,SIGNAL(startScan()),scan_thread,SLOT(scannerScanSN()));
    connect(robot_thread,SIGNAL(forShow(QString)),commDlg,SLOT(forShowInfo(QString)));
    connect(robot_thread,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(robot_thread,SIGNAL(errorMessage(QString)),errorDlg,SLOT(errorMessage(QString)));
    connect(robot_thread,SIGNAL(checkSnPass(QString)),this,SLOT(getSn(QString)));

    connect(this,SIGNAL(forShow(QString)),commDlg,SLOT(forShowInfo(QString)));
    connect(this,SIGNAL(manualScan()),scan_thread,SLOT(scannerScanSN()));
    connect(this,SIGNAL(sendErrorMsg(QString)),errorDlg,SLOT(errorMessage(QString)));

    connect(scanner,SIGNAL(serialReadReady()),scan_thread,SLOT(scannerReadSN()));

//    connect(scan_thread,SIGNAL(scanResult(QString)),this,SLOT(getSn(QString)));
    connect(scan_thread,SIGNAL(scanResult(QString)),robot_thread,SLOT(checkSn(QString)));
    connect(scan_thread,SIGNAL(scanError(QString)),this,SLOT(errorMessage(QString)));
    connect(scan_thread,SIGNAL(forShow(QString)),commDlg,SLOT(forShowInfo(QString)));
    connect(scan_thread,SIGNAL(scanError(QString)),errorDlg,SLOT(errorMessage(QString)));

    connect(robotServer,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(robotServer,SIGNAL(clientConnect(QString,int)),this,SLOT(robotConnected(QString,int)));
    connect(robotServer,SIGNAL(clientDisconnected(QString,int)),this,SLOT(robotDisconnected(QString,int)));
    connect(robotServer,SIGNAL(serverReadData(QString,int,QString)),robot_thread,SLOT(robotReadData(QString,int,QString)));
    connect(robotServer,SIGNAL(errorMessage(QString)),errorDlg,SLOT(errorMessage(QString)));

    init_Scanner_Robot();
    update_UI_show();
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
    for(int i=0; i<TYPE_TOTAL; i++)
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
    //设置状态栏
    statusBarLabel_Scanner = new QLabel(this);
    statusBarLabel_Robot = new QLabel(this);
    statusBarLabel_Scanner->setFrameStyle(QFrame::StyledPanel);
    statusBarLabel_Scanner->setTextFormat(Qt::RichText);
    statusBarLabel_Robot->setFrameStyle(QFrame::StyledPanel);
    statusBarLabel_Robot->setTextFormat(Qt::RichText);
    ui->statusBar->addPermanentWidget(statusBarLabel_Scanner);
    ui->statusBar->addPermanentWidget(statusBarLabel_Robot);

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);

    //Scanner
    QString portName = configRead->value(SCANNER_PORT_NAME).toString();
    int baudRate = configRead->value(SCANNER_BAUD_RATE).toString().toInt();
    int dataBits = configRead->value(SCANNER_DATA_BITS).toString().toInt();
    QString parityBits = configRead->value(SCANNER_PARITY_BITS).toString();
    QString stopBits = configRead->value(SCANNER_STOP_BITS).toString();
    if(!(scanner->openSerialPort(portName,baudRate,dataBits,parityBits,stopBits,true,true)))
    {
        QMessageBox::warning(this,tr("Error"),tr("Scanner initialize failed!\n"),QMessageBox::Ok);
        statusBarLabel_Scanner->setText("Scanner:Disconnected!");
        emit forShow(forShowString("Scanner initialize failed!\n"));
        emit sendErrorMsg("Scanner initialize failed!\n");
    }
    else
    {
        statusBarLabel_Scanner->setText(QString("Scanner:%1 Connected").arg(portName));
        emit forShow(forShowString(QString("Scanner:%1 Connected\n").arg(portName)));
    }

    //Robot
    QString address =configRead->value(SERVER_IP_ADDRESS).toString();
    quint16 port =(quint16) configRead->value(SERVER_PORT).toString().toInt();
    if(!robotServer->stratListen(address,port))
    {
        QMessageBox::warning(this,tr("Error"),tr("Robot initialize failed!\n"),QMessageBox::Ok);
        statusBarLabel_Robot->setText("Robot:Disconnected!");
        emit forShow(forShowString("Robot initialize failed!\n"));
        emit sendErrorMsg("Robot initialize failed!\n");
    }
    else
    {
        statusBarLabel_Robot->setText("Robot:Listening...");
        emit forShow(forShowString("Robot:Listening...\n"));
    }

    ui->comboBoxTypeSelect->clear();
    QString strTypeTemp = "";
    for(int i=0; i<TYPE_TOTAL; i++)
    {
        strTypeTemp = configRead->value(QString(MAIN_UI_TYPE).arg(i)).toString();
        if(""!=strTypeTemp)
        {
            ui->comboBoxTypeSelect->insertItem(i,strTypeTemp);
        }
    }

    delete configRead;
}

void ICT_UR10::update_UI_show()
{
    ui->labelTotalQty->setText(QString("%1").arg(totalQty));
    ui->labelPassQty->setText(QString("%1").arg(passQty));
    ui->labelFailQty->setText(QString("%1").arg(failQty));
    ui->labelYield->setText(QString("%1%").arg(yield*100));
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
    errorDlg->disEnable();
}

void ICT_UR10::Enable()
{
    ui->actionLogin->setText("LogOut");
    ui->actionScanner->setDisabled(false);
    ui->actionRobot->setDisabled(false);
    commDlg->Enable();
    errorDlg->Enable();
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
    emit forShow(forShowString(QString("Robot:%1 %2 Connected\n").arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP==IP && robotPort==QString("%1").arg(Port))
    {
        statusBarLabel_Robot->setText(QString("Robot:%1 %2 Connected").arg(IP).arg(Port));
    }
}

void ICT_UR10::robotDisconnected(QString IP, int Port)
{
    emit forShow(forShowString(QString("Robot:%1 %2 Disconnected!\n").arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP==IP && robotPort==QString("%1").arg(Port))
    {
        statusBarLabel_Robot->setText(QString("Robot:%1 %2 Disconnected!\n").arg(IP).arg(Port));
        QMessageBox::warning(this,"Warning",QString("Robot:%1 %2 Disconnected!\n").arg(IP).arg(Port),QMessageBox::Ok);
        emit sendErrorMsg(QString("Robot:%1 %2 Disconnected!\n").arg(IP).arg(Port));
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
        statusBarLabel_Scanner->setText(QString("Scanner:%1 Connected").arg(portName));
        emit forShow(forShowString(QString("Scanner:%1 Connected\n").arg(portName)));
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
    ui->tableWidgetResultList->setItem(row,0,new QTableWidgetItem(ui->comboBoxTypeSelect->currentText()));
    ui->tableWidgetResultList->setItem(row,1,new QTableWidgetItem(time.toString("yyyyMMdd")));
    ui->tableWidgetResultList->setItem(row,2,new QTableWidgetItem(time.toString("hh:mm:ss")));
    ui->tableWidgetResultList->setItem(row,3,new QTableWidgetItem(sn));
    ui->tableWidgetResultList->setItem(row,4,new QTableWidgetItem(result));
    ui->tableWidgetResultList->item(row, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 1)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 2)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 3)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetResultList->item(row, 4)->setTextAlignment(Qt::AlignCenter);

    totalQty++;//总数量加1
    if("PASS"==result.toUpper())
    {
        ui->tableWidgetResultList->item(row, 4)->setBackgroundColor(QColor(0, 255, 0));
        passQty++;//pass数量加1
    }
    else
    {
        if("FAIL"==result.toUpper())
        {
            ui->tableWidgetResultList->item(row, 4)->setBackgroundColor(QColor(255, 0, 0));
            failQty++;//fail数量加1
        }
    }
    yield = ((float)passQty)/((float)totalQty);//计算良率
    ui->tableWidgetResultList->scrollToBottom();

    //保存测试结果到csv文件
    QString datalist = QString("%1,%2,%3,%4,%5\n").arg(ui->comboBoxTypeSelect->currentText()).arg(time.toString("yyyyMMdd"))
            .arg(time.toString("hh:mm:ss")).arg(sn).arg(result);

    newFile();//检查本地数据文件夹是否存在，如果不存在则新建文件夹

    QFile csvFile(QString(LOCAL_DATA_FILE_NAME).arg(time.toString("yyyyMMdd")));
    if (csvFile.open(QIODevice::ReadWrite))
    {
        if(0 == csvFile.size())
        {
            csvFile.write(QString("Product Type,Date,Time,SN,TestResult\n").toLatin1());
        }
        csvFile.seek(csvFile.size());
        csvFile.write(datalist.toLatin1());
        csvFile.close();
    }
    ui->lineEditSN->clear();
    update_UI_show();
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

void ICT_UR10::on_pushButton_clicked()
{
    updateTestResult("SN1234567890","PASS");
}

void ICT_UR10::on_pushButton_2_clicked()
{
    updateTestResult("SN0987654321","FAIL");
}

void ICT_UR10::on_actionError_list_triggered()
{
    if(false == errorDlgIsShow)
    {
        errorDlg->show();
        errorDlgIsShow = true;
    }
    else
    {
        errorDlg->hide();
        errorDlgIsShow = false;
    }
}
