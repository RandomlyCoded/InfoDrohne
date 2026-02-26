#include "btdrone.h"
#include "btledrone.h"
#include "udpdrone.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    qmlRegisterType<randomly::UdpDrone>("DroneControl", 1, 0, "UdpDrone");
    qmlRegisterType<randomly::BtLEDrone>("DroneControl", 1, 0, "BtLEDrone");
    qmlRegisterType<randomly::BtDrone>("DroneControl", 1, 0, "BtDrone");

    engine.loadFromModule("Remote", "Main");

    return app.exec();
}
