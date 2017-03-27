#ifndef STATICNAME_H
#define STATICNAME_H

#define USER_ID                  ""
#define PASSWORD                 ""

#define CONFIG_FILE_NAME         "..\\path/Config.ini"
#define LOCAL_DATA_FOLDER_NAME   "..\\ICT Data"
#define LOCAL_DATA_FILE_NAME     "..\\ICT Data/ICT_UR10_test_result_%1.csv"

#define SCANNER_PORT_NAME        "/ScannerParameter/PortName"
#define SCANNER_BAUD_RATE        "/ScannerParameter/BaudRate"
#define SCANNER_DATA_BITS        "/ScannerParameter/DataBits"
#define SCANNER_PARITY_BITS      "/ScannerParameter/ParityBits"
#define SCANNER_STOP_BITS        "/ScannerParameter/StopBits"
#define SCANNER_PORT_NAME_CONTROL        "/ScannerParameter/PortNameControl"
#define SCANNER_BAUD_RATE_CONTROL        "/ScannerParameter/BaudRateControl"
#define SCANNER_DATA_BITS_CONTROL        "/ScannerParameter/DataBitsControl"
#define SCANNER_PARITY_BITS_CONTROL      "/ScannerParameter/ParityBitsControl"
#define SCANNER_STOP_BITS_CONTROL        "/ScannerParameter/StopBitsControl"

#define SERVER_IP_ADDRESS        "/RobotParameter/IPaddress"
#define SERVER_PORT              "/RobotParameter/Port"
#define ROBOT_IP                 "/RobotParameter/RobotIP"
#define ROBOT_PORT               "/RobotParameter/RobotPort"
#define ROBOT_TYPE_ENABLE        "/RobotParameter/RobotTypeEnable"
#define ROBOT_TYPE               "/RobotParameter/Type_%1"
#define ROBOT_PRO_NUM            "/RobotParameter/%1_Pro_Num"

#define TYPE_TOTAL               20
#define MAIN_UI_TYPE             "/MainUIParameter/Index%1"

#define ICT_LOCAL_IP                   "/ICT/IPaddress"
//A File
#define ICT_LOCAL_SN_FILE_NAME         "/ICT/SnFileName"
#define ICT_LOCAL_SN_NAME              "/ICT/SnName"
#define ICT_LOCAL_RECEIVE_FILE_NAME      "/ICT/ReceiveFileName"
#define ICT_LOCAL_RECEIVE_NAME           "/ICT/ReceiveName"
#define ICT_LOCAL_RUN_FILE_NAME      "/ICT/RunFileName"
#define ICT_LOCAL_RUN_NAME           "/ICT/RunName"
#define ICT_LOCAL_RESULT_FILE_NAME     "/ICT/ResultFileName"
#define ICT_LOCAL_RESULT_NAME          "/ICT/ResultName"
//F File
//#define ICT_LOCAL_ERROR_FILE_NAME      "/ICT/ErrorFileName"
//#define ICT_LOCAL_ERROR_NAME           "/ICT/ErrorName"
#define ICT_YELLOW_LIMIT     "/ICT/YellowLimit"
#define ICT_RED_LIMIT        "/ICT/RedLimit"
#define ICT_YIELD_BASE       "/ICT/YieldBase"
#define ICT_YIELD_LIMIT      "/ICT/YieldLimit"
#define ICT_TEST_TIMEOUT      "/ICT/TestTimeout"

#define PREFIX  "@"
#define SUFFIX  "\n"

//PREFIX_COMMAND_SUFFIX表示"@%1\r\n"
#define PREFIX_COMMAND  "@%1"
#define PREFIX_COMMAND_SUFFIX  "@%1\n"

//#define WEB_SERVEICE         "http://ezconsf.joytech.com.cn/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_TEST    "http://10.2.24.153/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_IP      "210.75.11.119"
//LIBRARY_ACTION_CODE
//#define GET_CONFIG           "GetConfigSetting"
//#define GET_SFDATA           "getSFData"
//#define CHECKIN              "CheckIn"
//#define TESTRESULT           "TestResult"

#define INFORMATION_FILE_NAME     "..\\log/Info_Log_%1.txt"
#define ERROR_FILE_NAME           "..\\log/Error_Log_%1.txt"
#define INFORMATION_FOLDER_NAME   "..\\log"

#define CONTROL_CHECK      "ck"
#define CONTROL_OUT1_ON    "on"
#define CONTROL_OUT1_OFF   "of"
#define CONTROL_OUT2_ON    "op"
#define CONTROL_OUT2_OFF   "cl"


#endif // STATICNAME_H
