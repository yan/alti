
TARGET = ble_debug
# CONFIG += console
CONFIG += c++11
# CONFIG -= app_bundle
#QT += quick
QT += core
QT += bluetooth
QT += gui
#QT -= gui

TEMPLATE = app

SOURCES += main.cpp \
    devicemanager.cc \
    ble_device.cc \
    alti.cc

HEADERS += \
    devicemanager.h \
    ble_device.h \
    alti.h

