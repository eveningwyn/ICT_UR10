#include "ict_ur10.h"
#include "ui_ict_ur10.h"
#include "staticname.h"
#include "scannerdialog.h"
#include "robotdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "language.h"
#include "debuglogindialog.h"
#include "debugdialog.h"
#include <QRegExp>
#include <QDesktopWidget>

#define PRO_VERSION  "V2.01Beta"
void ICT_UR10::on_actionAbout_triggered()
{
    QMessageBox::about(this,NULL,QString(tr("\nICT_UR10 version is %1.\n"
                                            "\nBuilt on 2018-03-05.\n"
                                            "\nThis version is not \"No Read\".\n"))
                       .arg(PRO_VERSION));
}

ICT_UR10::ICT_UR10(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ICT_UR10)
{
    ui->setupUi(this);
    ui->mainToolBar->setHidden(true);

    this->setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
    this->setFixedSize(QApplication::desktop()->width(),QApplication::desktop()->height()-70);
    QRegExp regExpInput("[0-9]+$");//[0-9]+$;^[0-9]*[1-9][0-9]*$
    ui->lineEdit_line_wait_time->setValidator(new QRegExpValidator(regExpInput,this));

    totalQty = 0;
    passQty = 0;
    failQty = 0;
    yield = 0.00;
    failCount = 0;
    totalQtyTemp = 0;
    passQtyTemp = 0;
    failQtyTemp = 0;
    yieldTemp = 0.00;
    UPH_Qty = 0;
    UPH_Pass = 0;
    UPH_Fail = 0;
    UPH_TimeStr = "";
    snTemp = "";
    QString UPH_end_TimeStr = "";
    UPH_Update_Infor(true,UPH_TimeStr,UPH_end_TimeStr,UPH_Pass,UPH_Fail,UPH_Qty);

    /*实例化scanner类，并移入子线程thread1中*/
    thread1 = new QThread;//实例化thread1线程对象
    scan_on_thread = new ScannerOnThread;//实例化scanner处理类对象
    scan_on_thread->moveToThread(thread1);//将scanner处理类对象放在线程中

    /*实例化robot类，并移入子线程thread2中*/
    thread2 = new QThread;//实例化thread2线程对象
    robot_on_thread = new RobotOnThread;//实例化robot处理类对象
    robot_on_thread->moveToThread(thread2);//将robot处理类对象放在线程中
    //robot_on_thread->moveToThread(thread1);//将robot处理类对象放在线程中

    /*实例化ICT类，并移入子线程thread3中*/
    thread3 = new QThread;//实例化thread3线程对象
    ict = new ICT_Test_Obj;
    ict->moveToThread(thread3);//将ict处理类对象放在线程中
    //ict->moveToThread(thread1);//将ict处理类对象放在线程中

    pWeb = new WebUploadObj(this);

    /*状态初始化*/
    robotIsInit = false;
    mainAutoMode = true;//初始化为自动模式

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
    connect(scan_on_thread,&ScannerOnThread::scanResult,ict,&ICT_Test_Obj::ict_Check_SN);
    connect(scan_on_thread,&ScannerOnThread::scanResult,this,&ICT_UR10::saveTempSn);
    connect(scan_on_thread,&ScannerOnThread::lineSensorStatus,robot_on_thread,&RobotOnThread::lineSensorStatus);
    connect(scan_on_thread,&ScannerOnThread::readSnDone,robot_on_thread,&RobotOnThread::readSnDone);

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
    connect(robot_on_thread,&RobotOnThread::cylinderUpDown,scan_on_thread,&ScannerOnThread::controlBoardWrite);
    connect(robot_on_thread,&RobotOnThread::robot_catchFail,ict,&ICT_Test_Obj::catchFail);
    connect(robot_on_thread,&RobotOnThread::change_auto_debug_label,this,&ICT_UR10::change_auto_debug_label);
    connect(robot_on_thread,&RobotOnThread::sortComplete,this,&ICT_UR10::UI_sortComplete);
    connect(robot_on_thread,&RobotOnThread::dashboard,this,&ICT_UR10::UI_dashboard);

    connect(ict,&ICT_Test_Obj::ict_Error_Msg,this,&ICT_UR10::showErrorMessage);
    connect(ict,&ICT_Test_Obj::ict_Error_Msg,errorDlg,&ErrorListDialog::recordErrorMessage);
    connect(ict,&ICT_Test_Obj::ictIsReady,this,&ICT_UR10::setIctReady);
    connect(ict,&ICT_Test_Obj::ict_Status,this,&ICT_UR10::update_ICT_Status);
    connect(ict,&ICT_Test_Obj::ictTestResult,robot_on_thread,&RobotOnThread::testResult);
    connect(ict,&ICT_Test_Obj::ict_Check_SN_Result,this,&ICT_UR10::getSn);
    connect(ict,&ICT_Test_Obj::ict_Check_SN_Result,robot_on_thread,&RobotOnThread::snCheckResult);
    connect(ict,&ICT_Test_Obj::ict_light_Red_Green_Yellow_Buzzer,robot_on_thread,&RobotOnThread::set_light_Red_Green_Yellow_Buzzer);
    connect(ict,&ICT_Test_Obj::openSwitch,scan_on_thread,&ScannerOnThread::controlBoardWrite);
    connect(ict,&ICT_Test_Obj::ict_setRunStatus,this,&ICT_UR10::runStatus);
    connect(ict,&ICT_Test_Obj::ict_testTimeout,robot_on_thread,&RobotOnThread::ict_testTimeout);
    connect(ict,&ICT_Test_Obj::forShow_To_Comm,commDlg,&CommunicationDialog::forShowInfo);

    connect(pWeb,&WebUploadObj::web_Error_Msg,this,&ICT_UR10::showErrorMessage);
    connect(pWeb,&WebUploadObj::web_Error_Msg,errorDlg,&ErrorListDialog::recordErrorMessage);

    connect(this,&ICT_UR10::manualSendMsg,robot_on_thread,&RobotOnThread::robotSendMsg);
    connect(this,&ICT_UR10::forShow,commDlg,&CommunicationDialog::forShowInfo);
    connect(this,&ICT_UR10::manualScan,scan_on_thread,&ScannerOnThread::scannerScanSN);
    connect(this,&ICT_UR10::sendErrorMsg,errorDlg,&ErrorListDialog::recordErrorMessage);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,scan_on_thread,&ScannerOnThread::init_Scanner);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,robot_on_thread,&RobotOnThread::init_Robot);
    connect(this,&ICT_UR10::init_scanner_robot_ict_mes,ict,&ICT_Test_Obj::init_ict);
    connect(this,&ICT_UR10::robotInit,robot_on_thread,&RobotOnThread::robot_Init);
    connect(this,&ICT_UR10::pcIsReady,robot_on_thread,&RobotOnThread::set_PC_Status);
    connect(this,&ICT_UR10::setType_Pro,robot_on_thread,&RobotOnThread::setPro_Num);
    connect(this,&ICT_UR10::set_ict_Enable,ict,&ICT_Test_Obj::set_ictEnable);
    connect(this,&ICT_UR10::robotSetAutoMode,robot_on_thread,&RobotOnThread::setRobotRunMode);
    connect(this,&ICT_UR10::manualSendMsg_controlBoard,scan_on_thread,&ScannerOnThread::controlBoardWrite);
    connect(this,&ICT_UR10::light_Red_Green_Yellow_Buzzer,robot_on_thread,&RobotOnThread::set_light_Red_Green_Yellow_Buzzer);
    connect(this,&ICT_UR10::robotPortExist,robot_on_thread,&RobotOnThread::setrobotPortExist);
    connect(this,&ICT_UR10::ui_robot_start,robot_on_thread,&RobotOnThread::robot_start);
    connect(this,&ICT_UR10::ui_robot_pause,robot_on_thread,&RobotOnThread::robot_pause);
    connect(this,&ICT_UR10::ui_robot_stop,robot_on_thread,&RobotOnThread::robot_stop);
    connect(this,&ICT_UR10::robotPortExist,scan_on_thread,&ScannerOnThread::robot_Connected);
    connect(this,&ICT_UR10::UI_msgUpload,pWeb,&WebUploadObj::msgUpload);

    thread1->start();//开启thread1的子线程
    thread2->start();//开启thread2的子线程
    thread3->start();//开启thread3的子线程

    init_UI();

//for Debug 20180227
//    QString sTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
//    emit UI_msgUpload("F",sTime,sTime,"Error PCB Unlevel on Flat");
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
    QString robotTypeEnable = configRead->value(ROBOT_TYPE_ENABLE).toString();
    QString strTypeTemp = "";
    for(int i=0; i<TYPE_TOTAL; i++)
    {
        strTypeTemp = configRead->value(QString(MAIN_UI_TYPE).arg(i)).toString();
        if(""!=strTypeTemp)
        {
            ui->comboBoxTypeSelect->insertItem(i,strTypeTemp);
        }
    }
    if("false"==robotTypeEnable.toLower())
    {
        ui->comboBoxTypeSelect->setDisabled(true);
        ui->pushButtonTypeSelect->setDisabled(true);
    }
    else
    {
        if("true"==robotTypeEnable.toLower())
        {
            ui->comboBoxTypeSelect->setDisabled(false);
            ui->pushButtonTypeSelect->setDisabled(false);
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
    ict_mes_Dlg.exec();
}

void ICT_UR10::init_UI()
{
    scannerIsReady = false;
    robotIsReady = false;
    ictIsReady = false;
    isAutoRun = false;
    ictEnable = true;

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
    QString robotTypeEnable = configRead->value(ROBOT_TYPE_ENABLE).toString();
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
    if("false"==robotTypeEnable)
    {
        ui->comboBoxTypeSelect->setDisabled(true);
        ui->pushButtonTypeSelect->setDisabled(true);
    }
    delete configRead;

    update_UI_show();
    disEnableUI();
    newFile();
}

void ICT_UR10::update_UI_show()
{
    QString time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    if(0==totalQty||1==totalQty)
    {
        ui->label_recordTime->setText(QString(tr("开始记录时间：%1")).arg(time));
    }
    ui->labelTotalQty->setText(QString("%1").arg(totalQty));
    ui->labelPassQty->setText(QString("%1").arg(passQty));
    ui->labelFailQty->setText(QString("%1").arg(failQty));
    ui->labelYield->setText(QString("%1%").arg(yield*100));
    if(0==totalQtyTemp||1==totalQtyTemp)
    {
        ui->label_recordTime_interim->setText(QString(tr("开始记录时间：%1")).arg(time));
    }
    ui->labelTotalQtyTemp->setText(QString("%1").arg(totalQtyTemp));
    ui->labelPassQtyTemp->setText(QString("%1").arg(passQtyTemp));
    ui->labelFailQtyTemp->setText(QString("%1").arg(failQtyTemp));
    ui->labelYieldTemp->setText(QString("%1%").arg(yieldTemp*100));
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

void ICT_UR10::manualStartScan(bool autoScan)
{
    emit manualScan(autoScan);
}

void ICT_UR10::manualSendMsg_robot(QString sendMsg)
{
    emit manualSendMsg(sendMsg);
}

void ICT_UR10::showErrorMessage(QString errorMsg)
{
    QRegExp errorRE("@Error (.*)\n");
    if(0 <= errorMsg.indexOf(errorRE))
    {
        emit ui_robot_pause();

        logToServerWeb("F",errorMsg);

        QString errorStr = errorRE.cap(1);
        errorStr.replace("\r","");
        errorStr.replace("\n","");

        uint alarmGrade = 1;//报警级别初始化为1级
        checkErrorMsg(errorStr,alarmGrade);
        QMessageBox::StandardButton rb;
        if(1==alarmGrade)
        {
            rb = QMessageBox::warning(this,tr("警告"),QString(tr("%1")).arg(errorStr),QMessageBox::Yes|QMessageBox::No);
            if(QMessageBox::Yes == rb)
            {
                emit ui_robot_start();
            }
            else
            {
                if(QMessageBox::No == rb)
                {
                    emit ui_robot_stop();
                }
            }
        }
        else
        {
            if(2==alarmGrade)
            {
                QMessageBox::warning(this,tr("警告"),QString(tr("%1")).arg(errorStr),QMessageBox::Ok);
                emit ui_robot_stop();
            }
            else
            {
                if(10==alarmGrade)
                {
                    //10级别只暂停，不做其他处理
                }
            }
        }
    }
    else
    {
        QMessageBox::warning(this,tr("警告"),QString(tr("%1")).arg(errorMsg),QMessageBox::Ok);
    }
}

void ICT_UR10::disEnableUI()
{
    ui->actionLogin->setText(tr("登入"));
    ui->actionScanner->setDisabled(true);
    ui->actionRobot->setDisabled(true);
    ui->actionICT_MES->setDisabled(true);
    ui->actionDebug->setDisabled(true);
    commDlg->disEnable(true);
    errorDlg->disEnable(true);
}

void ICT_UR10::Enable()
{
    ui->actionLogin->setText(tr("登出"));
    ui->actionScanner->setDisabled(false);
    ui->actionRobot->setDisabled(false);
    ui->actionICT_MES->setDisabled(false);
    ui->actionDebug->setDisabled(false);
    commDlg->disEnable(false);
    errorDlg->disEnable(false);
}

void ICT_UR10::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::warning(this,NULL,tr("退出当前应用程序?"),QMessageBox::Yes|QMessageBox::No);

    if (QMessageBox::No == button)
    {
        event->ignore();  //ignore
    }
    else if (QMessageBox::Yes == button)
    {
        event->accept();  //accept
    }
}

void ICT_UR10::robotConnected(QString IP, int Port)
{
    emit forShow(forShowString(QString(tr(" %1 %2 已连接\n")).arg(IP).arg(Port)));

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotIP = configRead->value(ROBOT_IP).toString();
    if(robotIP==IP)
    {
        UPH_Time = QDateTime::currentDateTime();
        if(""==UPH_TimeStr)
        {
            UPH_TimeStr = UPH_Time.toString("yyyy/MM/dd hh");
        }
        ui->label_UPH_time->setText(QString(tr("%1H~")).arg(UPH_Time.toString("yyyy/MM/dd hh")));
        configRead->setValue(ROBOT_PORT,QString("%1").arg(Port));
        statusBarLabel_Robot->setText(QString(tr("机器人:%1 %2 已连接")).arg(IP).arg(Port));
        emit robotPortExist(true);
        setRobotReady(true);
        ui->pushButton_Auto_Debug->setDisabled(false);
        ui->comboBox_Auto_Debug->setDisabled(false);
        if(false==robotIsInit)
        {
                emit robotInit();
        }
        on_pushButton_Auto_Debug_clicked();
    }
    delete configRead;
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
        emit robotPortExist(false);
        runStatus(false);
        robotInitStatus(false);
        setRobotReady(false);
        ui->pushButton_Auto_Debug->setDisabled(true);
        ui->comboBox_Auto_Debug->setDisabled(true);
        ui->comboBox_Auto_Debug->setCurrentIndex(0);//Robot断开时将模式选项设置为手动模式
        ui->label_Auto_Debug->setText(tr("调试状态..."));
        ui->label_Auto_Debug->setStyleSheet("");
        statusBarLabel_Robot->setText(QString(tr("机器人:%1 %2 已断开\n")).arg(IP).arg(Port));
        QMessageBox::warning(this,NULL,QString(tr("机器人:%1 %2 已断开!\n")).arg(IP).arg(Port),QMessageBox::Ok);
        emit sendErrorMsg(QString(tr("机器人:%1 %2 已断开!\n")).arg(IP).arg(Port));
    }
}

QString ICT_UR10::forShowString(QString str)
{
    QDateTime time = QDateTime::currentDateTime();
    str = time.toString("yyyy-MM-dd hh:mm:ss.zzz_") + str + "\n";
    return str;
}

void ICT_UR10::updateTestResult(QString sn, QString result)
{
    sn.replace("\r","");
    sn.replace("\n","");
    QString failCode = "";
    if(result.contains("FAIL"))
    {
        failCode = result;
        failCode.replace("FAIL,","");
        result = "FAIL";
    }
    ui->lineEditResult->setText(result.toUpper());
    if(99<=testCount)
    {
        ui->tableWidgetResultList->clearContents();
        ui->tableWidgetResultList->setRowCount(0);
        testCount = 0;
    }
    testCount++;

    QString pro_type = ui->comboBoxTypeSelect->currentText();
    if(pro_type.isEmpty())
    {
        pro_type = "Default";
    }

    ui->tableWidgetResultList->setRowCount(ui->tableWidgetResultList->rowCount() + 1);
    QDateTime time = QDateTime::currentDateTime();
    int row = ui->tableWidgetResultList->rowCount() - 1;
    ui->tableWidgetResultList->setItem(row,0,new QTableWidgetItem(pro_type));
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
    totalQtyTemp++;
    if("PASS"==result.toUpper())
    {
        ui->tableWidgetResultList->item(row, 4)->setBackgroundColor(QColor(0, 255, 0));
        ui->lineEditResult->setStyleSheet("background :rgb(0, 255, 0)");
        passQty++;//pass数量加1
        failCount = 0;
        passQtyTemp++;
    }
    else
    {
        if("FAIL"==result.toUpper())
        {
            ui->tableWidgetResultList->item(row, 4)->setBackgroundColor(QColor(255, 0, 0));
            ui->lineEditResult->setStyleSheet("background :rgb(255, 0, 0)");
            failQty++;//fail数量加1
            failCount++;
            failQtyTemp++;
        }
    }
    yield = ((float)passQty)/((float)totalQty);//计算良率
    yieldTemp = ((float)passQtyTemp)/((float)totalQtyTemp);//计算良率
    ui->tableWidgetResultList->scrollToBottom();

    //保存测试结果到csv文件
    QString datalist = QString("%1,%2,%3,%4,%5,%6,%7\n").arg(pro_type).arg(time.toString("yyyyMMdd"))
            .arg(time.toString("hh:mm:ss")).arg(sn).arg(result).arg(failCode).arg(totalQty);

    newFile();//检查本地数据文件夹是否存在，如果不存在则新建文件夹

    QFile csvFile(QString(LOCAL_DATA_FILE_NAME).arg(time.toString("yyyyMMdd")));
    if (csvFile.open(QIODevice::ReadWrite))
    {
        if(0 == csvFile.size())
        {
            csvFile.write(QString("ProductType,Date,Time,SN,TestResult,FailCode,ProductionTotalQty\n").toLatin1());
        }
        csvFile.seek(csvFile.size());
        csvFile.write(datalist.toLatin1());
        csvFile.close();
    }
    update_UI_show();

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString yellow_limit = configRead->value(ICT_YELLOW_LIMIT).toString();
    QString red_limit = configRead->value(ICT_RED_LIMIT).toString();
    QString yield_base = configRead->value(ICT_YIELD_BASE).toString();
    QString yield_limit = configRead->value(ICT_YIELD_LIMIT).toString();
    delete configRead;
    if((uint) yellow_limit.toInt()<=failCount ||(uint) red_limit.toInt()<=failCount)
    {
        if((uint) yellow_limit.toInt()<=failCount && (uint) red_limit.toInt()>failCount)
        {
            //打开三色灯_黄灯
            emit light_Red_Green_Yellow_Buzzer("Yellow light open");
            lightCount = 3;
        }
        else
        {
            if((uint) red_limit.toInt()<=failCount)
            {
                //打开三色灯_红灯
                emit light_Red_Green_Yellow_Buzzer("Red light open");
                lightCount = 1;
                if(QMessageBox::No==QMessageBox::warning(this,NULL,QString(tr("目前已经连续测试NG %1PCS产品,请维护人员检查测试机是否异常!\n"
                                                                                    "是否清零连续测试NG次数的统计？\n"
                                                                                    "不清零请选择Yes,清零请选择No\n")).arg(failCount),
                                                         QMessageBox::Yes|QMessageBox::No))
                {
                    failCount = 0;
                }
            }
        }
    }
    else
    {
        //关闭三色灯提示
        if(1==lightCount)
        {
            //关闭三色灯_红灯
            emit light_Red_Green_Yellow_Buzzer("Red light close");
        }
        else
        {
            if(3==lightCount)
            {
                //关闭三色灯_黄灯
                emit light_Red_Green_Yellow_Buzzer("Yellow light close");
            }
        }
        lightCount = 0;
    }

    if((uint) yield_base.toInt() <= totalQtyTemp && (uint) yield_limit.toFloat() > yieldTemp)
    {
        if(QMessageBox::Yes ==QMessageBox::warning(this,NULL,
                                                   QString(tr("已连续测试%1PCS产品，良率为%2%，良率值较低，请检查ICT测试机!\n"
                                                              "是否解除当前良率检测，重新统计？\n解除请选择Yes,不解除请选择No\n"))
                                                   .arg(totalQtyTemp).arg(yieldTemp*100),QMessageBox::Yes|QMessageBox::No))
        {
            passQtyTemp = 0;
            failQtyTemp = 0;
            totalQtyTemp = 0;
            yieldTemp = 0.00;
            failCount = 0;
            update_UI_show();
        }
    }
    snTemp = "";
}

void ICT_UR10::newFile()
{
    QDir *temp = new QDir;
    bool fileExist = temp->exists(LOCAL_DATA_FOLDER_NAME);
    if(!fileExist)
    {
        temp->mkdir(LOCAL_DATA_FOLDER_NAME);
    }
    delete temp;
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
    emit forShow(forShowString(status));
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
    PC_Status();
}

void ICT_UR10::runStatus(bool isAuto)
{
    isAutoRun = isAuto;
    if(true==isAutoRun)
    {
        this->statusBarLabel_Robot->setText(tr("机器人:自动运行中..."));
        //清空通讯log
        //emit forShow("UI clear...\n");
        m_sStartTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    else
    {
        this->statusBarLabel_Robot->setText(tr("机器人:手动运行中..."));
    }
    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString robotTypeEnable = configRead->value(ROBOT_TYPE_ENABLE).toString();
    delete configRead;
    if("true"==robotTypeEnable)
    {
        ui->comboBoxTypeSelect->setDisabled(isAuto);
        ui->pushButtonTypeSelect->setDisabled(isAuto);
    }
    if(tr("登出")==ui->actionLogin->text())
    {
        ui->actionScanner->setDisabled(isAuto);
        ui->actionRobot->setDisabled(isAuto);
        ui->actionICT_MES->setDisabled(isAuto);
        ui->actionDebug->setDisabled(isAuto);
//        commDlg->disEnable(isAuto);
        errorDlg->disEnable(isAuto);
    }
    ui->pushButton_Auto_Debug->setDisabled(isAuto);
    ui->comboBox_Auto_Debug->setDisabled(isAuto);
    if(true == isAuto)
    {
        ui->comboBox_Auto_Debug->setCurrentIndex(1);
    }
}

void ICT_UR10::on_actionDebug_triggered()
{
    if(true==mainAutoMode||tr("自动状态...")==ui->label_Auto_Debug->text())
    {
        QMessageBox::warning(this,NULL,tr("注意：请连接Robot并切换到手动模式后,再重新进入Debug界面!"));
        return;
    }
    DebugLoginDialog debugLoginDlg;
    if(QDialog::Accepted==debugLoginDlg.exec())
    {
        DebugDialog debugDlg(this);
        connect(&debugDlg,&DebugDialog::moveToScan,robot_on_thread,&RobotOnThread::debug_moveToScan);
        connect(&debugDlg,&DebugDialog::fixturePickup,robot_on_thread,&RobotOnThread::debug_fixturePickup);
        connect(&debugDlg,&DebugDialog::fixturePlace,robot_on_thread,&RobotOnThread::debug_fixturePlace);
        connect(&debugDlg,&DebugDialog::ictPlace,robot_on_thread,&RobotOnThread::debug_ictPlace);
        connect(&debugDlg,&DebugDialog::ictPickup,robot_on_thread,&RobotOnThread::debug_ictPickup);
        connect(&debugDlg,&DebugDialog::ictClose,robot_on_thread,&RobotOnThread::debug_ictClose);
        connect(&debugDlg,&DebugDialog::ictOpen,robot_on_thread,&RobotOnThread::debug_ictOpen);
        connect(&debugDlg,&DebugDialog::placeOKPos,robot_on_thread,&RobotOnThread::debug_placeOKPos);
        connect(&debugDlg,&DebugDialog::placeNGPos,robot_on_thread,&RobotOnThread::debug_placeNGPos);
        connect(&debugDlg,&DebugDialog::returnSafePos,robot_on_thread,&RobotOnThread::debug_returnSafePos);
        connect(&debugDlg,&DebugDialog::clawOpen,robot_on_thread,&RobotOnThread::debug_clawOpen);
        connect(&debugDlg,&DebugDialog::clawClose,robot_on_thread,&RobotOnThread::debug_clawClose);
        connect(&debugDlg,&DebugDialog::ict_start_test,ict,&ICT_Test_Obj::testStart);
        connect(&debugDlg,&DebugDialog::debug_CylinderUpDown,scan_on_thread,&ScannerOnThread::controlBoardWrite);
        connect(robot_on_thread,&RobotOnThread::debugRunDone,&debugDlg,&DebugDialog::runDone);
        connect(robot_on_thread,&RobotOnThread::dashboard,&debugDlg,&DebugDialog::Debug_dashboard);
        emit robotSetAutoMode(false);
        debugDlg.exec();
//        emit robotSetAutoMode(true);
    }
}

void ICT_UR10::on_pushButton_Auto_Debug_clicked()
{
    if(true == isAutoRun)
    {
        return;
    }
    bool modeTemp = mainAutoMode;
    if(tr("手动模式")==ui->comboBox_Auto_Debug->currentText())
    {
        mainAutoMode = false;
    }
    else
    {
        QMessageBox msgBox;
//        msgBox.setText(tr("安全警示!"));
        msgBox.setText(tr("<h1><font color=red>安全警示!</font></hi>"));
        msgBox.setInformativeText(tr("请通过手动操作机器人控制面板, 使得机器人安全回到初始位!\n"
                                     "请一定要进行此项操作, 否则容易出现安全问题!\n"
                                     "未进行此项操作请选择Yes, 已进行此项操作请选择No\n"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowFlags(Qt::FramelessWindowHint);//隐藏关闭按钮
        msgBox.setStyleSheet("background-color :rgb(240, 240, 240)");
        int rb = msgBox.exec();

        if(QMessageBox::No==rb)
        {
            if(tr("自动模式")==ui->comboBox_Auto_Debug->currentText())
            {
                mainAutoMode = true;
            }
        }
        else
        {
            ui->comboBox_Auto_Debug->setCurrentIndex(0);
            return;
        }
    }
    if(modeTemp!=mainAutoMode||false==mainAutoMode)
    {
        emit robotSetAutoMode(mainAutoMode);
    }
}

void ICT_UR10::on_pushButton_Robot_start_clicked()
{
    emit ui_robot_start();
}

void ICT_UR10::on_pushButton_Robot_pause_clicked()
{
    emit ui_robot_pause();
}

void ICT_UR10::on_pushButton_Robot_stop_clicked()
{
    emit ui_robot_pause();
    if(QMessageBox::Yes==QMessageBox::warning(this,NULL,tr("是否停止机器人运行？"),QMessageBox::Yes|QMessageBox::No))
    {
        emit ui_robot_stop();
    }
}

void ICT_UR10::checkErrorMsg(QString &errorMsgStr, uint &ALarmGrade)
{
    QFile errorFile("Error_list.txt");
    if(!errorFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ;
    }
    QTextStream txtInput(&errorFile);
    QString lineStr;
    while(!txtInput.atEnd())
    {
        lineStr = txtInput.readLine();
        if(lineStr.contains(errorMsgStr))
        {
            QRegExp errorRE("(.*),(.*),(.*)");
            if(0 <= lineStr.indexOf(errorRE))
            {
                ALarmGrade = errorRE.cap(1).toInt();
                errorMsgStr = errorRE.cap(3);
            }
            break;
        }
    }
    errorFile.close();
}

void ICT_UR10::change_auto_debug_label(QString labelStr)
{
    ui->label_Auto_Debug->setText(labelStr);
//    if(tr("登出")==ui->actionLogin->text())
//    {
//        ui->actionDebug->setDisabled(mainAutoMode);
//    }
    if(ui->label_Auto_Debug->text().contains(tr("调试状态")))
    {
        ui->label_Auto_Debug->setStyleSheet("");
    }
    else
    {
        ui->label_Auto_Debug->setStyleSheet("background-color: rgb(0, 255, 0)");
    }
}

void ICT_UR10::UI_show_error(QString errorStr)
{
    if(errorStr.contains("PC status?"))
    {
        ui->textBrowser_show_error->clear();
    }
    ui->textBrowser_show_error->moveCursor(QTextCursor::End);
    ui->textBrowser_show_error->insertPlainText(errorStr);
    ui->textBrowser_show_error->moveCursor(QTextCursor::End);
}

void ICT_UR10::UI_sortComplete(bool testResultPass)
{
    logToServerWeb("P","");

    QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    QString logIndex = configRead->value(LOG_INDEX).toString();
    configRead->setValue(LOG_INDEX,QString("%1").arg(logIndex.toInt()+1));
    delete configRead;
    QDateTime curTime_temp = QDateTime::currentDateTime();

    if(UPH_TimeStr!=curTime_temp.toString("yyyy/MM/dd hh"))
    {
        ui->label_UPH_time->setText(QString(tr("%1H~%2H")).arg(UPH_TimeStr).arg(curTime_temp.toString("yyyy/MM/dd hh")));
        ui->label_UPH_Qty->setText(QString("%1").arg(UPH_Qty));
        ui->label_UPH_Pass->setText(QString("%1").arg(UPH_Pass));
        ui->label_UPH_Fail->setText(QString("%1").arg(UPH_Fail));
        //保存测试结果到csv文件
        QString UPH_List = QString("%1,%2,%3,%4,%5\n").arg(UPH_TimeStr)
                .arg(curTime_temp.toString("yyyy/MM/dd hh")).arg(UPH_Pass).arg(UPH_Fail).arg(UPH_Qty);

        newFile();//检查本地数据文件夹是否存在，如果不存在则新建文件夹

        QFile UPH_csvFile(QString("..\\ICT Data/ICT_UR10_UPH_%1.csv").arg(curTime_temp.toString("yyyyMMdd")));
        if (UPH_csvFile.open(QIODevice::ReadWrite))
        {
            if(0 == UPH_csvFile.size())
            {
                UPH_csvFile.write(QString("StartTime,EndTime,PassQty,FailQty,UPH_Qty\n").toLatin1());
            }
            UPH_csvFile.seek(UPH_csvFile.size());
            UPH_csvFile.write(UPH_List.toLatin1());
            UPH_csvFile.close();
        }

        UPH_Qty = 0;
        UPH_Pass = 0;
        UPH_Fail = 0;
        UPH_Time = QDateTime::currentDateTime();
        UPH_TimeStr = UPH_Time.toString("yyyy/MM/dd hh");
    }
    if(true == testResultPass)
    {
        UPH_Pass++;
    }
    else
    {
        UPH_Fail++;
    }
    UPH_Qty = UPH_Pass + UPH_Fail;
    QString end_time = curTime_temp.toString("yyyy/MM/dd hh");
    UPH_Update_Infor(false,UPH_TimeStr,end_time,UPH_Pass,UPH_Fail,UPH_Qty);
}

void ICT_UR10::on_pushButtonTypeSelect_clicked()
{
    if(QMessageBox::No==QMessageBox::warning(this,NULL,tr("当前操作容易造成误操作,非专业人员禁止操作!\n是否停止切换程序号?"),QMessageBox::Yes|QMessageBox::No))
    {
        QString currType = ui->comboBoxTypeSelect->currentText();
        QSettings *configRead = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
        QString pro_num = configRead->value(QString(ROBOT_PRO_NUM).arg(currType)).toString();
        delete configRead;
        emit setType_Pro(pro_num);
    }
}

void ICT_UR10::UI_dashboard(int index, QString showStr)
{
    MainUI_mutex.lock();
    if(1==index)//更改启动按钮文字显示
    {
        ui->pushButton_Robot_start->setText(showStr);
    }
    else
    {
        if(2==index)//更改暂停按钮文字显示
        {
            ui->pushButton_Robot_pause->setText(showStr);
        }
        else
        {
            if(3==index)//更改停止按钮文字显示
            {
                ui->pushButton_Robot_stop->setText(showStr);
            }
        }
    }
    MainUI_mutex.unlock();
}

void ICT_UR10::UPH_Update_Infor(bool read_True, QString &timeStr, QString &end_timeStr, uint &passQ, uint &failQ, uint &tempQty)
{
    QString UPH_fileName = QString("UPH_Record.txt");
    QFile uph_file(UPH_fileName);
    QTextStream uph_file_in_out(&uph_file);
    if(true==read_True)
    {
        if(uph_file.open(QFile::ReadOnly | QIODevice::Text))
        {
            QString strTemp = uph_file_in_out.readAll();
            QRegExp uphRE("(.*),(.*),(.*),(.*),(.*)");
            if(0 <= strTemp.indexOf(uphRE))
            {
                timeStr = uphRE.cap(1);
                end_timeStr = uphRE.cap(2);
                passQ = uphRE.cap(3).toInt();
                failQ = uphRE.cap(4).toInt();
                tempQty = uphRE.cap(5).toInt();
            }
//            if(!uph_file.flush())
//            {
//                qDebug("UPH_Update_Infor_file flush error");
//            }
            uph_file.close();
        }
    }
    else
    {
        if(uph_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QString uph_Str_temp = QString("%1,%2,%3,%4,%5\n").arg(timeStr).arg(end_timeStr).arg(passQ).arg(failQ).arg(tempQty);
            uph_file_in_out << uph_Str_temp;
            if(!uph_file.flush())
            {
                qDebug("UPH_Update_Infor_file flush error");
            }
            uph_file.close();
        }
    }
}

void ICT_UR10::on_pushButton_cylinder_up_clicked()
{
    emit manualSendMsg_controlBoard(CONTROL_OUT1_ON);
}

void ICT_UR10::on_pushButton_cylinder_down_clicked()
{
    emit manualSendMsg_controlBoard(CONTROL_OUT1_OFF);
}

void ICT_UR10::on_lineEdit_line_wait_time_returnPressed()
{
    QString input = ui->lineEdit_line_wait_time->text();
    bool ok;
    int line_wait_time = input.toInt(&ok,10);
    if(!ok)
        line_wait_time = 1;
    QSettings *configWrite = new QSettings(CONFIG_FILE_NAME, QSettings::IniFormat);
    configWrite->setValue(SCANNER_LINE_WAIT_TIME,QString("%1").arg(line_wait_time));
    delete configWrite;
}

void ICT_UR10::saveTempSn(QString sn)
{
    snTemp = sn;
}

void ICT_UR10::logToServerWeb(const QString state, const QString errorCode)
{
    m_sEndTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString sError = errorCode;
    sError.replace("\r","");
    sError.replace("\n","");

    emit UI_msgUpload(state,m_sStartTime,m_sEndTime,sError);
//    m_sStartTime = "";
//    m_sEndTime = "";
}
