#ifndef STATICNAME_H
#define STATICNAME_H

#include <QString>

static QString USER_ID = "admin";
static QString PASSWORD = "admin";

static QString CONFIG_FILE_NAME                 = "..\\path/Config.ini";
static QString LOCAL_DATA_FOLDER_NAME           = "..\\ICT Data";
static QString LOCAL_DATA_FILE_NAME             = "..\\ICT Data/ICT_UR10_Test_Result_%1.csv";

static QString SCANNER_PORT_NAME            = "/ScannerParameter/PortName";
static QString SCANNER_BAUD_RATE            = "/ScannerParameter/BaudRate";
static QString SCANNER_DATA_BITS            = "/ScannerParameter/DataBits";
static QString SCANNER_PARITY_BITS          = "/ScannerParameter/ParityBits";
static QString SCANNER_STOP_BITS            = "/ScannerParameter/StopBits";
static QString SCANNER_PORT_NAME_CONTROL    = "/ScannerParameter/PortNameControl";
static QString SCANNER_BAUD_RATE_CONTROL    = "/ScannerParameter/BaudRateControl";
static QString SCANNER_DATA_BITS_CONTROL    = "/ScannerParameter/DataBitsControl";
static QString SCANNER_PARITY_BITS_CONTROL  = "/ScannerParameter/ParityBitsControl";
static QString SCANNER_STOP_BITS_CONTROL    = "/ScannerParameter/StopBitsControl";
static QString SCANNER_SCAN_TIMES           = "/ScannerParameter/ScanTimes";

static QString SERVER_IP_ADDRESS   = "/RobotParameter/IPaddress";
static QString SERVER_PORT         = "/RobotParameter/Port";
static QString ROBOT_IP            = "/RobotParameter/RobotIP";
static QString ROBOT_PORT          = "/RobotParameter/RobotPort";
static QString ROBOT_TYPE_ENABLE   = "/RobotParameter/RobotTypeEnable";
static QString ROBOT_TYPE          = "/RobotParameter/Type_%1";
static QString ROBOT_PRO_NUM       = "/RobotParameter/%1_Pro_Num";

static int TYPE_TOTAL              = 20;
static QString MAIN_UI_TYPE        = "/MainUIParameter/Index%1";

static QString ICT_LOCAL_IP        = "/ICT/IPaddress";
static QString ICT_LOCAL_DRIVE     = "/ICT/Drive";

static QString ICT_LOCAL_SN_FILE_NAME       = "/ICT/SnFileName";
static QString ICT_LOCAL_SN_NAME            = "/ICT/SnName";
static QString ICT_LOCAL_RECEIVE_FILE_NAME  = "/ICT/ReceiveFileName";
static QString ICT_LOCAL_RECEIVE_NAME       = "/ICT/ReceiveName";
static QString ICT_LOCAL_RUN_FILE_NAME      = "/ICT/RunFileName";
static QString ICT_LOCAL_RUN_NAME           = "/ICT/RunName";
static QString ICT_LOCAL_RESULT_FILE_NAME   = "/ICT/ResultFileName";
static QString ICT_LOCAL_RESULT_NAME        = "/ICT/ResultName";
//#define ICT_LOCAL_ERROR_FILE_NAME         "/ICT/ErrorFileName"
//#define ICT_LOCAL_ERROR_NAME              "/ICT/ErrorName"
static QString ICT_YELLOW_LIMIT             = "/ICT/YellowLimit";
static QString ICT_RED_LIMIT                = "/ICT/RedLimit";
static QString ICT_YIELD_BASE               = "/ICT/YieldBase";
static QString ICT_YIELD_LIMIT              = "/ICT/YieldLimit";
static QString ICT_TEST_TIMEOUT             = "/ICT/TestTimeout";
static QString ICT_OPEN_WAIT_TIME           = "/ICT/OpenWaitTime";

static QString PREFIX = "@";
static QString SUFFIX = "\n";
//PREFIX_COMMAND_SUFFIX表示"@%1\r\n"
static QString PREFIX_COMMAND        = "@%1";
static QString PREFIX_COMMAND_SUFFIX = "@%1\n";

//#define WEB_SERVEICE         "http://ezconsf.joytech.com.cn/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_TEST    "http://10.2.24.153/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_IP      "210.75.11.119"
//LIBRARY_ACTION_CODE
//#define GET_CONFIG           "GetConfigSetting"
//#define GET_SFDATA           "getSFData"
//#define CHECKIN              "CheckIn"
//#define TESTRESULT           "TestResult"

static QString INFORMATION_FILE_NAME   = "..\\log/%1/Info_Log_%2.txt";
static QString ERROR_FILE_NAME         = "..\\log/%1/Error_Log_%2.txt";
static QString INFORMATION_FOLDER_NAME = "..\\log";
static QString LOG_INDEX               = "/LOG/LogIndex";

static QString CONTROL_CHECK     = "ck";
static QString CONTROL_OUT1_ON   = "on";
static QString CONTROL_OUT1_OFF  = "of";
static QString CONTROL_OUT2_ON   = "op";
static QString CONTROL_OUT2_OFF  = "cl";


#endif // STATICNAME_H
