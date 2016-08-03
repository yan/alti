#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>

#include <stdlib.h>

#include "alticontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");

    QString foo = "AltiSensorPacket";
    qmlRegisterUncreatableType<AltiSensorPacket>("org.srtd", 1, 0, "AltiSensorPacket", foo);

    AltiController controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controller", &controller);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    //controller.tryConnect();

    return app.exec();
}
