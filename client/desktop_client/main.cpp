#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "devicemanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    DeviceManager dm;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("deviceManager", &dm);
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}

