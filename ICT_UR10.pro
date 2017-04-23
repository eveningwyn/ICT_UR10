#-------------------------------------------------
#
# Project created by QtCreator 2017-02-26T10:25:06
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += network
QT       += core xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ICT_UR10
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS += -ldbghelp

SOURCES += main.cpp\
        ict_ur10.cpp \
    scannerdialog.cpp \
    robotdialog.cpp \
    communicationdialog.cpp \
    logindialog.cpp \
    tcpipclient.cpp \
    tcpipserver.cpp \
    errorlistdialog.cpp \
    scanneronthread.cpp \
    robotonthread.cpp \
    serialportobj.cpp \
    ict_test_obj.cpp \
    ict_mes_dialog.cpp \
    debugdialog.cpp \
    debuglogindialog.cpp

HEADERS  += ict_ur10.h \
    scannerdialog.h \
    robotdialog.h \
    communicationdialog.h \
    logindialog.h \
    tcpipclient.h \
    tcpipserver.h \
    staticname.h \
    errorlistdialog.h \
    scanneronthread.h \
    robotonthread.h \
    serialportobj.h \
    language.h \
    ict_test_obj.h \
    ict_mes_dialog.h \
    debugdialog.h \
    debuglogindialog.h

FORMS    += ict_ur10.ui \
    scannerdialog.ui \
    robotdialog.ui \
    communicationdialog.ui \
    logindialog.ui \
    errorlistdialog.ui \
    ict_mes_dialog.ui \
    debugdialog.ui \
    debuglogindialog.ui

RESOURCES += \
    resource.qrc
