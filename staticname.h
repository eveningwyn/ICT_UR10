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
#define ICT_LOCAL_TEST_PATH      "//192.168.3.123/data/%1"
#define ICT_LOCAL_RESULT_NAME    "result.txt"
#define ICT_LOCAL_STATUS_NAME    "status.txt"


#endif // STATICNAME_H
