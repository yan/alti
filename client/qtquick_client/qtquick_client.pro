QT += qml quick quickcontrols2 bluetooth positioning

CONFIG += c++14 warn_on

SOURCES += main.cpp \
    alticontroller.cc \
    altisensorpacket.cpp \
    globals.cc \
    alticonfig.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

INCLUDEPATH += ../../firmware/include

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    alticontroller.h \
    altisensorpacket.h \
    datareader.h \
    globals.h \
    alticonfig.h
    #altidevice.h

