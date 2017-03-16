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
#include "language.h"

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

    /*实例化scanner类，并移入子线程thread1中*/
    thread1 = new QThread;//实例化thread1线程对象
    scan_on_thread = new ScannerOnThread;//实例化scanner处理类对象
    scan_on_thread->moveToThread(thread1);//将scanner处理类对象放在线程中

    /*实例化robot类，并移入子线程thread2中*/
    thread2 = new QThread;//实例化thread2线程对象
    robot_on_thread = new RobotOnThread;//实例化scanner处理类对象
    robot_on_thread->moveToThread(thread2);//将scanner处理类对象放在线程中

    /*实例化ICT类，并移入子线程thread3中*/
    thread3 = new QThread;//实例化thread3线程对象
    ict = new ICT_Test_Obj;
    ict->moveToThread(thread3);//将ict处理类对象放在线程中

    /*实例化MES类，并移入子线程thread1中*/
    mesSystem = new MesCheckObj;
    mesSystem->moveToThread(thread1);//将mes处理类对象放在线程中

    /*状态初始化*/
    robotIsInit = false;

    /*实例化对话框类对象*/
    commDlg = new CommunicationDialog(this);
    commDlgIsShow = false;
    loginDlg = new LoginDialog(this);
    loginDlgIsShow = false;
    errorDlg = new ErrorListDialog(this);
    errorDlgIsShow = false;

    testCount = 0;//测试次数，用于对主界面的计数

    ui->tableWidgetResultList->setColumnWidth(3,200);

    connect(thread1,&QThread::finished,thread1,&QThread::deleteLater);

    connect(thread2,&QThread::finished,thread2,&QThread::deleteLater);

    connect(thread3,&QThread::finished,thread3,&QThread::deleteLater);

    connect(scan_on_thread,&ScannerOnThread::scanner_Status,this,&ICT_UR10::update_Scanner_Status);
    connect(scan_on_thread,&ScannerOnThread::scanner_Error_Msg,this,&ICT_UR10::showErrorMessage);
    connect(scan_on_thread,&ScannerOnThread::scanner_Error_Msg,errorDlg,&ErrorListDialog::recordErrorMessage);
    connect(scan_on_thread,&ScannerOnThread::scannerIsReady,this,&ICT_UR10::setScannerReady);
    connect(scan_on_thread,&ScannerOnThread::forShow_To_Comm,commDlg,&CommunicationDialog::forShowInfo);
    connect(scan_on_thread,&ScannerOnThread::scanError,robot_on_thread,&RobotOnThread::scanError);
    connect(scan_on_thread,&ScannerOnThread::scanResult,mesSystem,&MesCheckObj::checkSn);

    connect(robot_on_thread,&RobotOnThread::robot_Status,this,&ICT_UR10::update_Robot_Status);
    connect(robot_on_thread,&RobotOnThread::robot_Error_Msg,this,&ICT_UR10::showErrorMessage);
    connect(robot_on_thread,&RobotOnThread::robot_Error_Msg,errorDlg,&ErrorListDialog::recordErrorMessage);
    connect(robot_on_thread,&RobotOnThread::startScan,scan_on_thread,&ScannerOnThread::scannerScanSN);
    connect(robot_on_thread,&RobotOnThread::forShow_To_Comm,commDlg,&CommunicationDialog::forShowInfo);
    connect(robot_on_thread,&RobotOnThread::robotConnected,this,&ICT_UR10::robotConnected);
    connect(robot_on_thread,&RobotOnThread::robotDisconnected,this,&ICT_UR10::robotDisconnected);
    connect(robot_on_thread,&RobotOnThread::robotReady,this,&ICT_UR10::setRobotReady);
    connect(robot_on_thread,&RobotOnThread::startTest,ict,&ICT_Test_Obj::testStart);
    connect(robot_on_thread,&RobotOnThread::recordTestResult,this,&ICT_UR10::updateTestResult);
    connect(robot_on_thread,&RobotOnThread::setRunStatus,this,&ICT_UR10::runStatus);

    connect(ict,&ICT_Test_Obj::ict_Error_Msg,this,&ICT_UR10::showErrorMessage);
    connect(ict,&ICT_Test_Obj::ict_Error_Msg,errorDlg,&ErrorListDialog::recordErrorMessage);
    connect(ict,&ICT_Test_Obj::ictIsReady,this,&ICT_UR10::setIctReady);
    connect(ict,&ICT_Test_Obj::ict_Status,this,&ICT_UR10::update_ICT_Status);
    connect(ict,&ICT_Test_Obj::ictTestResult,robot_on_thread,&RobotOnThread::testResult);

    connect(mesSystem,&MesCheckObj::checkSnResult,this,&ICT_UR10::getSn);
    connect(mesSystem,&MesCheckObj::checkSnResult,robot_on_thread,&RobotOnThread::snCheckResult);
    connect(mesSystem,&MesCheckObj::mes_Error_Msg,this,&ICT_UR10::showErrorMessage);
    connect(mesSystem,&MesCheckObj::mes_Error_Msg,errorDlg,&ErrorListDialog::recordErrorMessage);

    connect(this,&ICT_UR10::manualSendMsg,robot_on_thread,&RobotOnThread::robotSendMsg);
    connect(this,&ICT_UR10::forShow,commDlg,&CommunicationDialog::forShowInfo);
    connect(this,&ICT_UR10::manualScan,scan_on_thread,&ScannerOnThread::scannerScanSN);
    connect(this,&ICT_UR10::sendErrorMsg,errorDlg,&ErrorListDialog::recordErrorMessage);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,scan_on_thread,&ScannerOnThread::init_Scanner);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,robot_on_thread,&RobotOnThread::init_Robot);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,ict,&ICT_Test_Obj::init_ict);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,mesSystem,&MesCheckObj::init_mes);
    connect(this,&ICT_UR10::robotInit,robot_on_thread,&RobotOnThread::robot_Init);
    connect(this,&ICT_UR10::pcIsReady,robot_on_thread,&RobotOnThread::set_PC_Status);
    connect(this,&ICT_UR10::setType_Pro,robot_on_thread,&RobotOnThread::setPro_Num);
    connect(this,&ICT_UR10::set_ict_Enable,ict,&ICT_Test_Obj::set_ictEnable);
    connect(this,&ICT_UR10::set_ict_Enable,robot_on_thread,&RobotOnThread::set_ictEnable);
    connect(this,&ICT_UR10::set_mes_Enable,mesSystem,&MesCheckObj::set_mes_enable);
    connect(this,&ICT_UR10::get_MES_Config,mesSystem,&MesCheckObj::get_mes_config);

    thread1->start();//开启thread1的子线程
    thread2->start();//开启thread2的子线程
    thread3->start();//开启thread3的子线程

    init_UI();
}

ICT_UR10::~ICT_UR10()
{
    if(thread1->isRunning())
        thread1->quit();
    if(thread2->isRunning())
        thread2->quit();
    if(thread3->isRunning())
        thread3->quit();
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

void ICT_UR10::on_actionICT_MES_triggered()
{
    ICT_MES_Dialog ict_mes_Dlg(this);
    connect(mesSystem,&MesCheckObj::rtnConfig,&ict_mes_Dlg,&ICT_MES_Dialog::showConfig);
    ict_mes_Dlg.exec();
}

void ICT_UR10::init_UI()
{
    scannerIsReady = false;
    robotIsReady = false;
    ictIsReady = false;
//    isAutoRun = false;

    //设置状态栏
    statusBarLabel_Scanner = new QLabel(this);
    statusBarLabel_Scanner->setFrameStyle(QFrame::StyledPanel);
    statusBarLabel_Scanner->setTextFormat(Qt::RichText);
    ui->statusBar->addPermanentWidget(statusBarLabel_Scanner);
    statusBarLabel_Robot = new QLabel(this);
    statusBarLabel_Robot->setFrameStyle(QFrame::StyledPanel);
    statusBarLabel_Robot->setTextFormat(Qt::RichText);
    ui->statusBar->addPermanentWidget(statusBarLabel_Robot);
    statusBarLabel_ICT = new QLabel(this);
    statusBarLabel_ICT->setFrameStyle(QFrame::StyledPanel);
    statusBarLabel_ICT->setTextFormat(Qt::RichText);
    ui->statusBar->addPermanentWidget(statusBarLabel_ICT);

    this->statusBarLabel_ICT->setText(tr("ICT:已断开"));

    emit init_scanner_robot_ict_mes();//初始化类对象

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
    delete configRead;

    update_UI_show();
    disEnableUI();
    newFile();
}

void ICT_UR10::update_UI_show()
{
    ui->labelTotalQty->setText(QString("%1").arg(totalQty));
    ui->labelPassQty->setText(QString("%1").arg(passQty));
    ui->labelFailQty->setText(QString("%1").arg(failQty));
    ui->labelYield->setText(QString("%1%").arg(yield*100));
}

void ICT_UR10::getSn(QString sn, bool checkResult)
{
    if(true == checkResult)
    {
        sn.replace("\r","");
        sn.replace("\n","");
        ui->lineEditSN->setText(sn);
        ui->lineEditResult->clear();
        ui->lineEditResult->setStyleSheet("background :rgb(255, 255, 255)");
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

void ICT_UR10::showErrorMessage(QString errorMsg)
{
    QMessageBox::warning(this,tr("错误信息"),QString(tr("%1")).arg(errorMsg),QMessageBox::Ok);
}

void ICT_UR10::disEnableUI()
{
    ui->actionLogin->setText(tr("登入"));
    ui->actionScanner->setDisabled(true);
    ui->actionRobot->setDisabled(true);
    ui->actionICT_MES->setDisabled(true);
    commDlg->disEnable();
    errorDlg->disEnable();
}

void ICT_UR10::Enable()
{
    ui->actionLogin->setText(tr("登出"));
    ui->actionScanner->setDisabled(false);
    ui->actionRobot->setDisabled(false);
    ui->actionICT_MES->setDisabled(false);
    commDlg->Enable();
    errorDlg->Enable();
}

void ICT_UR10::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::warning(this,tr("退出应用"),tr("退出应用程序？"),QMessageBox::Yes|QMessageBox::No);

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
    emit forShow(forShowString(QString(tr(" %1 %2 已连接\n")).arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP==IP && robotPort==QString("%1").arg(Port))
    {
        statusBarLabel_Robot->setText(QString(tr("机器人:%1 %2 已连接")).arg(IP).arg(Port));
        setRobotReady(true);
        on_comboBoxTypeSelect_currentTextChanged(ui->comboBoxTypeSelect->currentText());
        if(false==robotIsInit)
        {
            if(QMessageBox::Yes==QMessageBox::warning(this,tr("安全提示"),tr("是否让机器人进行初始化操作？\n在选择“Yes”之前，请确认机器人周边环境安全！"),
                                                      QMessageBox::Yes|QMessageBox::No))
            {
                emit robotInit();
            }
        }
    }
}

void ICT_UR10::robotDisconnected(QString IP, int Port)
{
    emit forShow(forShowString(QString(tr(" %1 %2 已断开!\n")).arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    QString robotPort = configRead->value(ROBOT_PORT).toString();
    delete configRead;
    if(robotIP==IP && robotPort==QString("%1").arg(Port))
    {
        robotInitStatus(false);
        setRobotReady(false);
        statusBarLabel_Robot->setText(QString(tr("机器人:%1 %2 已断开\n")).arg(IP).arg(Port));
        QMessageBox::warning(this,"警告",QString(tr("机器人:%1 %2 已断开!\n")).arg(IP).arg(Port),QMessageBox::Ok);
        emit sendErrorMsg(QString(tr("机器人:%1 %2 已断开!\n")).arg(IP).arg(Port));
    }
}

QString ICT_UR10::forShowString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + str + "\r\n";
    return str;
}

void ICT_UR10::updateTestResult(QString sn, QString result)
{
    sn.replace("\r","");
    sn.replace("\n","");
    ui->lineEditResult->setText(result.toUpper());
    if(100<=testCount)
    {
        ui->tableWidgetResultList->clearContents();
        ui->tableWidgetResultList->setRowCount(0);
        testCount = 0;
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
        ui->lineEditResult->setStyleSheet("background :rgb(0, 255, 0)");
        passQty++;//pass数量加1
    }
    else
    {
        if("FAIL"==result.toUpper())
        {
            ui->tableWidgetResultList->item(row, 4)->setBackgroundColor(QColor(255, 0, 0));
            ui->lineEditResult->setStyleSheet("background :rgb(255, 0, 0)");
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
//    ui->lineEditSN->clear();
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

void ICT_UR10::update_Scanner_Status(QString status)
{
    this->statusBarLabel_Scanner->setText(status);
}

void ICT_UR10::update_Robot_Status(QString status)
{
    this->statusBarLabel_Robot->setText(status);
}

void ICT_UR10::update_ICT_Status(QString status)
{
    this->statusBarLabel_ICT->setText(status);
}

void ICT_UR10::robotInitStatus(bool status)
{
    robotIsInit = status;
}

void ICT_UR10::PC_Status()
{
    bool isReady = false;
    if(true==scannerIsReady && true==robotIsReady && true==ictIsReady)
    {
        isReady = true;
    }
    emit pcIsReady(isReady);
}

void ICT_UR10::setScannerReady(bool isReady)
{
    scannerIsReady = isReady;
    PC_Status();
}

void ICT_UR10::setRobotReady(bool isReady)
{
    robotIsReady = isReady;
    PC_Status();
}

void ICT_UR10::setIctReady(bool isReady)
{
    ictIsReady = isReady;
    ictIsReady = true;//调试用-------------------------------------------
    PC_Status();
}

void ICT_UR10::on_comboBoxTypeSelect_currentTextChanged(const QString &arg1)
{
    if(true == robotIsReady)
    {
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString pro_num = configRead->value(QString(ROBOT_PRO_NUM).arg(arg1)).toString();
        delete configRead;
        emit setType_Pro(pro_num);
    }
}

void ICT_UR10::runStatus(bool isAuto)
{
//    isAutoRun = isAuto;
    ui->comboBoxTypeSelect->setDisabled(isAuto);
    ui->actionScanner->setDisabled(isAuto);
    ui->actionRobot->setDisabled(isAuto);
    ui->actionICT_MES->setDisabled(isAuto);
    ui->actionLogin->setDisabled(isAuto);
}
