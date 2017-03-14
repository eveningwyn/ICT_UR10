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

#define SERVER_IP_ADDRESS        "/RobotParameter/IPaddress"
#define SERVER_PORT              "/RobotParameter/Port"
#define ROBOT_IP                 "/RobotParameter/RobotIP"
#define ROBOT_PORT               "/RobotParameter/RobotPort"
#define ROBOT_TYPE               "/RobotParameter/Type_%1"
#define ROBOT_PRO_NUM            "/RobotParameter/%1_Pro_Num"

#define TYPE_TOTAL               20
#define MAIN_UI_TYPE             "/MainUIParameter/Index%1"

#define ICT_LOCAL_IP             "192.168.3.123"
#define ICT_LOCAL_FILE_PATH      "//192.168.3.123/data/%1"
#define ICT_LOCAL_SN_NAME        "SN.txt"
#define ICT_LOCAL_RESULT_NAME    "Result.txt"
#define ICT_LOCAL_OPEN_NAME      "Open.txt"
#define ICT_LOCAL_CLOSE_NAME     "Close.txt"
#define ICT_LOCAL_ERROR_NAME     "Error.txt"

#define PREFIX  "@"
#define SUFFIX  "##"

//PREFIX_COMMAND_SUFFIX表示"@%1\r\n"
#define PREFIX_COMMAND  "@%1"
#define PREFIX_COMMAND_SUFFIX  "@%1##"

#define WEB_SERVEICE         "http://ezconsf.joytech.com.cn/WSCenter/WSCenterEntrance.svc"
#define WEB_SERVEICE_TEST    "http://10.2.24.153/WSCenter/WSCenterEntrance.svc"
//#define WEB_SERVEICE_IP      "210.75.11.119"
//LIBRARY_ACTION_CODE
#define GET_CONFIG           "GetConfigSetting"
//#define GET_SFDATA           "getSFData"
#define CHECKIN              "CheckIn"
//#define TESTRESULT           "TestResult"


#endif // STATICNAME_H
