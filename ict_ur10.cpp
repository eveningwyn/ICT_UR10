#include "ict_ur10.h"
#include "ui_ict_ur10.h"
#include "scannerdialog.h"
#include "robotdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
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

    /*实例化scanner类，并移入子线程scanThread中*/
    scanThread = new QThread;//实例化scan线程对象
    scan_on_thread = new ScannerOnThread;//实例化scanner处理类对象
    scan_on_thread->moveToThread(scanThread);//将scanner处理类对象放在线程中
    scantimer = new QTimer(this);
    scanCount = 0;
    connect(scantimer,&QTimer::timeout,this,&ICT_UR10::timerTimeOut);

    /*实例化robot类，并移入子线程robotThread中*/
    robotThread = new QThread;//实例化robot线程对象
    robot_on_thread = new RobotOnThread;//实例化scanner处理类对象
    robot_on_thread->moveToThread(robotThread);//将scanner处理类对象放在线程中

    /*实例化对话框类对象*/
    commDlg = new CommunicationDialog(this);
    commDlgIsShow = false;
    loginDlg = new LoginDialog(this);
    loginDlgIsShow = false;
    errorDlg = new ErrorListDialog(this);
    errorDlgIsShow = false;

    testCount = 0;

    ui->tableWidgetResultList->setColumnWidth(3,200);

    connect(scan_on_thread,&ScannerOnThread::scanner_Status,this,&ICT_UR10::update_Scanner_Status);
    connect(scan_on_thread,&ScannerOnThread::scanner_Error_Msg,this,&ICT_UR10::errorMessage);
    connect(scan_on_thread,&ScannerOnThread::scanner_Error_Msg,errorDlg,&ErrorListDialog::errorMessage);
    connect(scan_on_thread,&ScannerOnThread::forShow_To_Comm,commDlg,&CommunicationDialog::forShowInfo);
    connect(scan_on_thread,&ScannerOnThread::scanResult,robot_on_thread,&RobotOnThread::checkSn);
    connect(scan_on_thread,&ScannerOnThread::start_timer,this,&ICT_UR10::start_scanner_timer);
    connect(scan_on_thread,&ScannerOnThread::stop_timer,this,&ICT_UR10::stop_scanner_timer);

    connect(robot_on_thread,&RobotOnThread::robot_Status,this,&ICT_UR10::update_Robot_Status);
    connect(robot_on_thread,&RobotOnThread::robot_Error_Msg,this,&ICT_UR10::errorMessage);
    connect(robot_on_thread,&RobotOnThread::robot_Error_Msg,errorDlg,&ErrorListDialog::errorMessage);
    connect(robot_on_thread,&RobotOnThread::startScan,scan_on_thread,&ScannerOnThread::scannerScanSN);
    connect(robot_on_thread,&RobotOnThread::forShow_To_Comm,commDlg,&CommunicationDialog::forShowInfo);
    connect(robot_on_thread,&RobotOnThread::checkSnPass,this,&ICT_UR10::getSn);
    connect(robot_on_thread,&RobotOnThread::robotConnected,this,&ICT_UR10::robotConnected);
    connect(robot_on_thread,&RobotOnThread::robotDisconnected,this,&ICT_UR10::robotDisconnected);

    connect(this,&ICT_UR10::manualSendMsg,robot_on_thread,&RobotOnThread::robotSendMsg);
    connect(this,&ICT_UR10::forShow,commDlg,&CommunicationDialog::forShowInfo);
    connect(this,&ICT_UR10::manualScan,scan_on_thread,&ScannerOnThread::scannerScanSN);
    connect(this,&ICT_UR10::sendErrorMsg,errorDlg,&ErrorListDialog::errorMessage);
    connect(this,&ICT_UR10::setCanScan,scan_on_thread,&ScannerOnThread::setCanScan);
    connect(this,&ICT_UR10::init_scanner_robot,scan_on_thread,&ScannerOnThread::init_Scanner);
    connect(this,&ICT_UR10::init_scanner_robot,robot_on_thread,&RobotOnThread::init_Robot);

    scanThread->start();//开启scanner的子线程
    robotThread->start();//开启robot的子线程

    init_UI();
    update_UI_show();
    disEnableUI();
    newFile();
}

ICT_UR10::~ICT_UR10()
{
    if(scanThread->isRunning())
        scanThread->quit();
    if(robotThread->isRunning())
        robotThread->quit();
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

void ICT_UR10::init_UI()
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

    emit init_scanner_robot();
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

void ICT_UR10::manualSendMsg_robot(QString sendMsg)
{
    emit manualSendMsg(sendMsg);
}

void ICT_UR10::errorMessage(QString errorMsg)
{
    QMessageBox::warning(this,tr("Error"),QString("%1").arg(errorMsg),QMessageBox::Ok);
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

//void ICT_UR10::updateScannerStatue(QString portName,bool connected)
//{
//    if(true == connected)
//    {
//        statusBarLabel_Scanner->setText(QString("Scanner:%1 Connected").arg(portName));
//        emit forShow(forShowString(QString("Scanner:%1 Connected\n").arg(portName)));
//        return;
//    }
//}

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

void ICT_UR10::update_Scanner_Status(QString status)
{
    this->statusBarLabel_Scanner->setText(status);
}

void ICT_UR10::update_Robot_Status(QString status)
{
    this->statusBarLabel_Robot->setText(status);
}

void ICT_UR10::timerTimeOut()
{
    if(scantimer->isActive())
        scantimer->stop();

    if(3 > scanCount)//3次扫描机会
    {
        emit setCanScan();
        manualStartScan();
        return;
    }

    //三次扫描失败
    scanCount = 0;
    QString errorMsg = "Scan barcode timeout!\n";
    errorMessage(errorMsg);
    emit setCanScan();
    emit sendErrorMsg(errorMsg);
}

void ICT_UR10::start_scanner_timer()
{
    scantimer->start(2200);
    scanCount++;
}

void ICT_UR10::stop_scanner_timer()
{
    if(scantimer->isActive())
        scantimer->stop();
    scanCount = 0;
}

