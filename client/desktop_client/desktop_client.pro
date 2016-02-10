TEMPLATE = app

QT += qml quick bluetooth positioning location
CONFIG += c++11

SOURCES += main.cpp \
    devicemanager.cpp \
    alti.cc

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
#include(deployment.pri)

HEADERS += \
    devicemanager.h \
    alti.h \
    sensor_packet.h

