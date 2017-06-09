#ifndef STATICNAME_H
#define STATICNAME_H

#include <QString>

//static QString USER_ID = "admin";
//static QString PASSWORD = "admin";
#define USER_ID  "admin"
#define PASSWORD  "admin"

//static QString CONFIG_FILE_NAME                 = "..\\path/Config.ini";
//static QString LOCAL_DATA_FOLDER_NAME           = "..\\ICT Data";
//static QString LOCAL_DATA_FILE_NAME             = "..\\ICT Data/ICT_UR10_Test_Result_%1.csv";
#define CONFIG_FILE_NAME                  "..\\path/Config.ini"
#define LOCAL_DATA_FOLDER_NAME            "..\\ICT Data"
#define LOCAL_DATA_FILE_NAME              "..\\ICT Data/ICT_UR10_Test_Result_%1.csv"

//static QString SCANNER_PORT_NAME            = "/ScannerParameter/PortName";
//static QString SCANNER_BAUD_RATE            = "/ScannerParameter/BaudRate";
//static QString SCANNER_DATA_BITS            = "/ScannerParameter/DataBits";
//static QString SCANNER_PARITY_BITS          = "/ScannerParameter/ParityBits";
//static QString SCANNER_STOP_BITS            = "/ScannerParameter/StopBits";
//static QString SCANNER_PORT_NAME_CONTROL    = "/ScannerParameter/PortNameControl";
//static QString SCANNER_BAUD_RATE_CONTROL    = "/ScannerParameter/BaudRateControl";
//static QString SCANNER_DATA_BITS_CONTROL    = "/ScannerParameter/DataBitsControl";
//static QString SCANNER_PARITY_BITS_CONTROL  = "/ScannerParameter/ParityBitsControl";
//static QString SCANNER_STOP_BITS_CONTROL    = "/ScannerParameter/StopBitsControl";
//static QString SCANNER_SCAN_TIMES           = "/ScannerParameter/ScanTimes";
#define SCANNER_PORT_NAME             "/ScannerParameter/PortName"
#define SCANNER_BAUD_RATE             "/ScannerParameter/BaudRate"
#define SCANNER_DATA_BITS             "/ScannerParameter/DataBits"
#define SCANNER_PARITY_BITS           "/ScannerParameter/ParityBits"
#define SCANNER_STOP_BITS             "/ScannerParameter/StopBits"
#define SCANNER_PORT_NAME_CONTROL     "/ScannerParameter/PortNameControl"
#define SCANNER_BAUD_RATE_CONTROL     "/ScannerParameter/BaudRateControl"
#define SCANNER_DATA_BITS_CONTROL     "/ScannerParameter/DataBitsControl"
#define SCANNER_PARITY_BITS_CONTROL   "/ScannerParameter/ParityBitsControl"
#define SCANNER_STOP_BITS_CONTROL     "/ScannerParameter/StopBitsControl"
#define SCANNER_SCAN_TIMES            "/ScannerParameter/ScanTimes"
#define SCANNER_LINE_WAIT_TIME            "/ScannerParameter/LineWaitTime"

//static QString SERVER_IP_ADDRESS   = "/RobotParameter/IPaddress";
//static QString SERVER_PORT         = "/RobotParameter/Port";
//static QString ROBOT_IP            = "/RobotParameter/RobotIP";
//static QString ROBOT_PORT          = "/RobotParameter/RobotPort";
//static QString ROBOT_TYPE_ENABLE   = "/RobotParameter/RobotTypeEnable";
//static QString ROBOT_TYPE          = "/RobotParameter/Type_%1";
//static QString ROBOT_PRO_NUM       = "/RobotParameter/%1_Pro_Num";
#define SERVER_IP_ADDRESS    "/RobotParameter/IPaddress"
#define SERVER_PORT          "/RobotParameter/Port"
#define ROBOT_IP             "/RobotParameter/RobotIP"
#define ROBOT_PORT           "/RobotParameter/RobotPort"
#define ROBOT_TYPE_ENABLE    "/RobotParameter/RobotTypeEnable"
#define ROBOT_TYPE           "/RobotParameter/Type_%1"
#define ROBOT_PRO_NUM        "/RobotParameter/%1_Pro_Num"

//static int TYPE_TOTAL              = 20;
//static QString MAIN_UI_TYPE        = "/MainUIParameter/Index%1";
#define TYPE_TOTAL           20
#define MAIN_UI_TYPE         "/MainUIParameter/Index%1"

//static QString ICT_LOCAL_IP        = "/ICT/IPaddress";
//static QString ICT_LOCAL_DRIVE     = "/ICT/Drive";
#define ICT_LOCAL_IP         "/ICT/IPaddress"
#define ICT_LOCAL_DRIVE      "/ICT/Drive"

//static QString ICT_LOCAL_SN_FILE_NAME       = "/ICT/SnFileName";
//static QString ICT_LOCAL_SN_NAME            = "/ICT/SnName";
//static QString ICT_LOCAL_RECEIVE_FILE_NAME  = "/ICT/ReceiveFileName";
//static QString ICT_LOCAL_RECEIVE_NAME       = "/ICT/ReceiveName";
//static QString ICT_LOCAL_RUN_FILE_NAME      = "/ICT/RunFileName";
//static QString ICT_LOCAL_RUN_NAME           = "/ICT/RunName";
//static QString ICT_LOCAL_RESULT_FILE_NAME   = "/ICT/ResultFileName";
//static QString ICT_LOCAL_RESULT_NAME        = "/ICT/ResultName";
#define ICT_LOCAL_SN_FILE_NAME        "/ICT/SnFileName"
#define ICT_LOCAL_SN_NAME             "/ICT/SnName"
#define ICT_LOCAL_RECEIVE_FILE_NAME   "/ICT/ReceiveFileName"
#define ICT_LOCAL_RECEIVE_NAME        "/ICT/ReceiveName"
#define ICT_LOCAL_RUN_FILE_NAME       "/ICT/RunFileName"
#define ICT_LOCAL_RUN_NAME            "/ICT/RunName"
#define ICT_LOCAL_RESULT_FILE_NAME    "/ICT/ResultFileName"
#define ICT_LOCAL_RESULT_NAME         "/ICT/ResultName"
//#define ICT_LOCAL_ERROR_FILE_NAME         "/ICT/ErrorFileName"
//#define ICT_LOCAL_ERROR_NAME              "/ICT/ErrorName"
//static QString ICT_YELLOW_LIMIT             = "/ICT/YellowLimit";
//static QString ICT_RED_LIMIT                = "/ICT/RedLimit";
//static QString ICT_YIELD_BASE               = "/ICT/YieldBase";
//static QString ICT_YIELD_LIMIT              = "/ICT/YieldLimit";
//static QString ICT_TEST_TIMEOUT             = "/ICT/TestTimeout";
//static QString ICT_OPEN_WAIT_TIME           = "/ICT/OpenWaitTime";
#define ICT_YELLOW_LIMIT              "/ICT/YellowLimit"
#define ICT_RED_LIMIT                 "/ICT/RedLimit"
#define ICT_YIELD_BASE                "/ICT/YieldBase"
#define ICT_YIELD_LIMIT               "/ICT/YieldLimit"
#define ICT_TEST_TIMEOUT              "/ICT/TestTimeout"
#define ICT_OPEN_WAIT_TIME            "/ICT/OpenWaitTime"


//static QString PREFIX = "@";
//static QString SUFFIX = "\n";
//--/*PREFIX_COMMAND_SUFFIX表示"@%1\r\n"*/
//static QString PREFIX_COMMAND        = "@%1";
//static QString PREFIX_COMMAND_SUFFIX = "@%1\n";

#define PREFIX  "@"
#define SUFFIX  "\n"
#define PREFIX_COMMAND         "@%1"
#define PREFIX_COMMAND_SUFFIX  "@%1\n"

//#define WEB_SERVEICE         "http://ezconsf.joytech.com.cn/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_TEST    "http://10.2.24.153/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_IP      "210.75.11.119"
//LIBRARY_ACTION_CODE
//#define GET_CONFIG           "GetConfigSetting"
//#define GET_SFDATA           "getSFData"
//#define CHECKIN              "CheckIn"
//#define TESTRESULT           "TestResult"

//static QString INFORMATION_FILE_NAME   = "..\\log/%1/Info_Log_%2.txt";
//static QString ERROR_FILE_NAME         = "..\\log/%1/Error_Log_%2.txt";
//static QString INFORMATION_FOLDER_NAME = "..\\log";
//static QString LOG_INDEX               = "/LOG/LogIndex";

#define INFORMATION_FILE_NAME    "..\\log/%1/Info_Log_%2.txt"
#define ERROR_FILE_NAME          "..\\log/%1/Error_Log_%2.txt"
#define INFORMATION_FOLDER_NAME  "..\\log"
#define LOG_INDEX                "/LOG/LogIndex"

//static QString CONTROL_CHECK     = "ck";
//static QString CONTROL_OUT1_ON   = "on";
//static QString CONTROL_OUT1_OFF  = "of";
//static QString CONTROL_OUT2_ON   = "op";
//static QString CONTROL_OUT2_OFF  = "cl";

#define CONTROL_CHECK      "ck"
#define CONTROL_OUT1_ON    "on"
#define CONTROL_OUT1_OFF   "of"
#define CONTROL_OUT2_ON    "op"
#define CONTROL_OUT2_OFF   "cl"


#endif // STATICNAME_H
