TEMPLATE = app
TARGET = IntegratedImpedanceSensingSystem
QT += core \
    gui \
    xml
CONFIG += console

#PATH_SETTINGS_SET = COMPUTER_1
#PATH_SETTINGS_SET = COMPUTER_2
PATH_SETTINGS_SET = COMPUTER_3

contains ( PATH_SETTINGS_SET, COMPUTER_1) {
    QEXTSERIALPORT_PATH = "C:/Documents and Settings/Instrument/My Documents/PManandhar/qextserialport"
    MCCDAQ_PATH = "C:/Program Files/Measurement Computing/DAQ/C"
    VISA_PATH   = "C:/Program Files/IVI Foundation/VISA/WinNT"
}

contains ( PATH_SETTINGS_SET, COMPUTER_2) {
    QEXTSERIALPORT_PATH = "C:/EclipseWorkspace/qextserialport"
    QEXTSERIALPORT_BUILD_PATH = $$QEXTSERIALPORT_PATH
    MCCDAQ_PATH = "C:/Users/Public/Documents/Measurement Computing/DAQ/C"
    VISA_PATH   = "C:/Program Files/IVI Foundation/VISA/Win64"
}

contains ( PATH_SETTINGS_SET, COMPUTER_3) {
    QEXTSERIALPORT_PATH = "C:/qextserialport"
    QEXTSERIALPORT_BUILD_PATH = "C:/qextserialport-build-desktop"
    MCCDAQ_PATH = "C:/Users/Public/Documents/Measurement Computing/DAQ/C"
    VISA_PATH   = "C:/Program Files/IVI Foundation/VISA/Win64"
}

LIBS        += $$QEXTSERIALPORT_BUILD_PATH/src/build/libqextserialportd1.a
INCLUDEPATH += $$QEXTSERIALPORT_PATH/src
INCLUDEPATH += $$VISA_PATH/Include
INCLUDEPATH += $$MCCDAQ_PATH


win32:LIBS += $$VISA_PATH/agvisa/lib/msc/visa32.lib
win32:LIBS += $$MCCDAQ_PATH/cbw32.lib

HEADERS += LoggerTime.h ImpedanceCalculator.h \
    RobotControl.h MeasurementThread.h ChannelInformation.h \
    improbotexp.h MeasurementProgDlg.h ExperimentSettings.h \
    integratedimpedancesensingsystem.h Measurement.h \
    Global.h FrequencyListGenerator.h MeasurementLoop.h \
    eit8electrodedlg.h \
    eitmeasurementloop.h \
    AbstractMeasurementLoop.h \
    EITMeasurementThread.h \
    singlefreqimpdlg.h
SOURCES += LoggerTime.cpp ImpedanceCalculator.cpp \
    RobotControl.cpp MeasurementThread.cpp ChannelInformation.cpp \
    improbotexp.cpp ExperimentSettings.cpp \
    main.cpp MeasurementProgDlg.cpp Measurement.cpp \
    integratedimpedancesensingsystem.cpp \
    Global.cpp FrequencyListGenerator.cpp MeasurementLoop.cpp \
    eit8electrodedlg.cpp \
    eitmeasurementloop.cpp \
    AbstractMeasurementLoop.cpp \
    EITMeasurementThread.cpp \
    singlefreqimpdlg.cpp
FORMS += improbotexp.ui \
    integratedimpedancesensingsystem.ui MeasurementProgDlg.ui \
    eit8electrodedlg.ui \
    singlefreqimpdlg.ui
RESOURCES += 
