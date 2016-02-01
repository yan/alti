#include <QCoreApplication>
#include <QGuiApplication>
#include <QObject>
#include <QLoggingCategory>
#include <chrono>
#include <thread>

#include "devicemanager.h"

int main(int argc, char *argv[])
{
    // QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));
    QGuiApplication app(argc, argv);

    DeviceManager manager;
    manager.start();

    return app.exec();
}

