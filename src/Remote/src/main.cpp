#include "backend.h"
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

    auto udpDrone = new randomly::UdpDrone(&app);
    auto btleDrone = new randomly::BtLEDrone(&app);
    auto btDrone = new randomly::BtDrone(&app);
    auto backend = new randomly::Backend(btleDrone, udpDrone, btDrone, &app);

    btleDrone->setObjectName("btleDrone");
    btDrone->setObjectName("btDrone");
    udpDrone->setObjectName("udpDrone");

    qmlRegisterSingletonInstance<randomly::UdpDrone>("DroneControl", 1, 0, "UdpDrone", udpDrone);
    qmlRegisterSingletonInstance<randomly::BtLEDrone>("DroneControl", 1, 0, "BtLEDrone", btleDrone);
    qmlRegisterSingletonInstance<randomly::Backend>("DroneControl", 1, 0, "Backend", backend);

    engine.loadFromModule("Remote", "Main");

    return app.exec();
}
